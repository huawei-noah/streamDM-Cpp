/*
 * Copyright (C) 2015 Holmes Team at HUAWEI Noah's Ark Lab.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "ArffReader.h"

REGISTER_CLASS(ArffReader);
REGISTER_COMMAND_LINE_PARAMETER(ArffReader, "{\"type\":\"Reader\","
		"\"name\":\"ArffReader\","
		"\"parameter\":{"
		"\"-d\":\"DynamicAttributes\""
		"}}"
		"");

ArffReader::ArffReader() {
	this->mInstanceInformation = new InstanceInformation();
	this->mFile = new ifstream();
	this->mAttributes = nullptr;
	this->mDynamicAttributes = false;
	this->mHeaderLoaded = false;
}

ArffReader::~ArffReader() {
	renew();
	delete mInstanceInformation;
	delete mFile;
}

void ArffReader::doSetParams() {
	this->mDynamicAttributes = (this->getParam("DynamicAttributes", 0) == 1);
}

ArffReader::LocalAttributes::LocalAttributes() {
	this->count = 0;
}

ArffReader::LocalAttributes::LocalAttributes(InstanceInformation* ii) {
	this->count = ii->getNumberInputAttributes() + 1;
	this->isNumeric.resize(this->count);
	for (int i = 0; i < this->count - 1; i++) {
		this->isNumeric[i] = ii->getInputAttribute(i)->isNumeric();
		int numberAttributeValues = ii->getInputAttribute(i)->numberValues();
		for (int j = 0; j < numberAttributeValues; j++) {
			this->values[i][ii->getInputAttribute(i)->getValue(j)] = j;
		}
	}
	this->isNumeric[this->count - 1] = false;
	int numberAttributeValues = ii->getNumberClasses();
	for (int j = 0; j < numberAttributeValues; j++) {
		this->values[this->count - 1][ii->getOutputAttribute(0)->getValue(j)] =
				j;
	}
}

void ArffReader::LocalAttributes::saveToInstanceInformation(
		InstanceInformation*& ii) {
	for (unsigned int i = 0; i < values.size(); i++) {
		vector<string> vec;
		for (auto iter = values[i].begin(); iter != values[i].end(); iter++) {
			vec.push_back(iter->first);
		}
		if (i != values.size() - 1) {
			if (isNumeric[i]) {
				ii->addAttribute(new Attribute(), i);
			} else {
				ii->addAttribute(new Attribute(vec), i);
			}
		} else {
			ii->addClass(new Attribute(vec), 0);
		}
	}
}

double ArffReader::LocalAttributes::getIndex(const int& sequence,
		const string& value) {
	if (value == "?") {
		return NAN;
	}
	if (this->isNumeric[sequence]) {
		double continuousValue;
		sscanf(value.c_str(), "%lf", &continuousValue);
		return continuousValue;
		// return atof(value.c_str());
	} else {
		if (this->values[sequence].find(value)
				!= this->values[sequence].end()) {
			return this->values[sequence][value];
		} else {
			return -1; // this condition must refresh the attributes, modify it later.
		}
	}
}

//void ArffReader::LocalAttributes::appendClass(const string& value){
//	this->isNumeric.push_back(false);
//	this->count = this->isNumeric.size();
//
//	stringstream ss(value);
//	int counter = 0;
//	string item;
//	while(getline(ss, item, ',')) {
//		item.erase(0, item.find_first_not_of(" \r\n\t"));
//		item.erase(item.find_first_not_of(" \r\n\t"));
//		this->values[this->count-1][item] = counter;
//		counter++;		
//	}
//
//	this->isClassSaved = true;
//}

void ArffReader::LocalAttributes::appendNumericAttribute() {
	this->isNumeric.push_back(true);
	this->count = this->isNumeric.size();
}

void ArffReader::LocalAttributes::appendDiscreteAttribute(const string& value) {
	this->isNumeric.push_back(false);
	this->count = this->isNumeric.size();

	stringstream ss(value);
	int counter = 0;
	string item;
	while (getline(ss, item, ',')) {
		item.erase(0, item.find_first_not_of(" \r\n\t"));
		item.erase(item.find_last_not_of(" \r\n\t") + 1);
		this->values[this->count - 1][item] = counter;
		counter++;
	}
}

Instance* ArffReader::nextInstance() {
	if (!this->hasNextInstance()) {
		return nullptr;
	}

	Instance* ret = mNextInstance;
	readData();

	return ret;
}

bool ArffReader::setFile(const string& fileName) {

	renew();

	if (!checkFileExist(fileName)) {
		return false;
	}

	if (!openFile(fileName)) {
		return false;
	}

	if (!readHeader()) {
		return false;
	}

	mHeaderLoaded = true;

	// read first row
	if (!readData()) {
		return false;
	}

	return true;
}

void ArffReader::renew() {
	if (mHeaderLoaded) {
		if (mFile->is_open()) {
			mFile->close();
		}

		delete mInstanceInformation;
		mInstanceInformation = new InstanceInformation();
		if (this->mHasNextInstance) {
			delete this->mNextInstance;
		}
	}

	if (this->mAttributes != nullptr) {
		delete this->mAttributes;
		this->mAttributes = nullptr;
	}

	this->mHasNextInstance = false;
	mHeaderLoaded = false;
}

bool ArffReader::checkFileExist(const string& fileName) {
	if (access(fileName.c_str(), 0) != 0) {
		return false;
	}
	return true;
}

bool ArffReader::readHeader() {

	mHasNextInstance = false;
	mHeaderLoaded = false;

	string s = "";
	mAttributes = new LocalAttributes();

	while (!mHeaderLoaded) {
		if (!getline(*mFile, s)) {
			return false;
		}

		// mHeaderLoad is changed to true in parseHeaderData() 
		// when all header information is read
		// and find the string "@DATA"
		if (!parseHeaderData(s)) {
			return false;
		};
	}

	return mHeaderLoaded;
}

/*
 % 1. Title: Iris Plants Database
 %
 % 2. Sources:
 %      (a) Creator: R.A. Fisher
 %      (b) Donor: Michael Marshall (MARSHALL%PLU@io.arc.nasa.gov)
 %      (c) Date: July, 1988
 %
 @RELATION iris

 @ATTRIBUTE sepallength  NUMERIC
 @ATTRIBUTE sepalwidth   NUMERIC
 @ATTRIBUTE petallength  NUMERIC
 @ATTRIBUTE petalwidth   NUMERIC
 @ATTRIBUTE class        {Iris-setosa,Iris-versicolor,Iris-virginica}

 @DATA
 5.1,3.5,1.4,0.2,Iris-setosa
 */
bool ArffReader::parseHeaderData(const string& row) {
	if (row.size() == 0) {
		return true;
	}

	if (row[row.find_first_not_of(" ")] == '%') {
		return true;
	}

	if (row.find_first_not_of(" \n\r\t") == row.npos) {
		return true;
	}

	string s = row;
	s.erase(0, s.find_first_not_of(" \n\r\t"));
	s.erase(s.find_last_not_of(" \n\r\t") + 1);
	int len = 0;

	len = string("@RELATION ").size();
	if (s.size() > (size_t) len
			&& strcasecmp(s.substr(0, len).c_str(), "@RELATION ") == 0) {
		return true;
	}

	len = string("@ATTRIBUTE ").size();
	if (s.size() > (size_t) len
			&& strcasecmp(s.substr(0, len).c_str(), "@ATTRIBUTE ") == 0) {
		s.erase(0, len);
		s.erase(0, s.find_first_not_of(" \n\r\t"));
		//len = string("class").size();
		//if (strcmpIgnoreCase(s.substr(0, len).c_str(), "class") == 0) {
		//	s = s.substr(s.find_last_of("{"), s.size());
		//	mAttributes->appendClass(s);
		//	return true;
		//} else {
		len = string(" NUMERIC").size();
		if (s.size() > (size_t) len
				&& strcasecmp(s.substr(s.size() - len, len).c_str(), " NUMERIC")
						== 0) {
			mAttributes->appendNumericAttribute();
			return true;
		}

		len = string(" REAL").size();
		if (s.size() > (size_t) len
				&& strcasecmp(s.substr(s.size() - len, len).c_str(), " REAL")
						== 0) {
			mAttributes->appendNumericAttribute();
			return true;
		}

		{
			int pos = s.find_last_of("{");
			s = s.substr(pos + 1, s.size() - pos - 2);
			mAttributes->appendDiscreteAttribute(s);
			return true;
		}
		//}
		return true;
	}

	len = string("@DATA").size();
	if (s.size() >= (size_t) len
			&& strcasecmp(s.substr(0, len).c_str(), "@DATA") == 0) {
//		if (mAttributes->count > 0 && mAttributes->isClassSaved) {
		if (mAttributes->count > 0) {
			mHeaderLoaded = true;
			mAttributes->saveToInstanceInformation(mInstanceInformation);
			return true;
		} else {
			return false;
		}
	}

	LOG_WARN("Symbal not defind. (%s)", s.c_str());
	return true;
}

bool ArffReader::readData() {
	if (!mHeaderLoaded) {
		LOG_ERROR("You must call setFile() function before read data.");
		return false;
	}

	mHasNextInstance = false;
	string s = "";

	while (getline(*mFile, s)) {
		if (s.size() != 0 && s[0] != '%') {
			if (this->mDynamicAttributes) {
				return this->inputForDynamicAttributes(s) == 0;
			} else {
				return input(s) == 0;
			}
		}
	}

	return true;
}

int ArffReader::input(string& s) {

	int index = 0;
	string item = "";
	double d = 0;
	this->mHasNextInstance = false;

	stringstream ss(s);
	vector<double> labels(1);
	vector<double> values(mAttributes->count - 1);
	while (getline(ss, item, ',')) {
//		item.erase(0, item.find_first_not_of(" \r\n\t"));
//        item.erase(item.find_last_not_of(" \r\n\t") + 1);
		d = mAttributes->getIndex(index, item);
		if (index == this->mAttributes->count - 1) {
			labels[0] = d;
			break;
		} else {
			values[index] = d;
		}
		index++;
	}

	if (index != this->mAttributes->count - 1) {
		LOG_ERROR("Data error (%s).", s.c_str());
		return -1;
	}

	this->mHasNextInstance = true;
	this->mNextInstance = new DenseInstance();
	this->mNextInstance->setInstanceInformation(this->mInstanceInformation);
	this->mNextInstance->addLabels(labels);
	this->mNextInstance->addValues(values);

	return 0;
}

int ArffReader::inputForDynamicAttributes(string& s) {
	int index = 0;
	string item = "";
	double d = 0;
	this->mHasNextInstance = false;

	stringstream ss(s);
	vector<string> line;
	while (getline(ss, item, ',')) {
		line.push_back(item);
	}
	if (line.size()
			!= (this->mInstanceInformation->getNumberInputAttributes() + 1)) {
		vector<string> vecAttrValue = {"0", "1"};
		this->mInstanceInformation->setNumberInputAttributes(line.size() - 1, vecAttrValue);
		delete this->mAttributes;
		this->mAttributes = new LocalAttributes(this->mInstanceInformation);
	}

	vector<double> labels(1);
	vector<double> values(mAttributes->count - 1);

	for (unsigned int i = 0; i < line.size(); i++) {
		d = mAttributes->getIndex(index, line[i]);
		if (index == this->mAttributes->count - 1) {
			labels[0] = d;
			break;
		} else {
			values[index] = d;
		}
		index++;
	}

	if (index != this->mAttributes->count - 1) {
		LOG_ERROR("Data number error (%s).", s.c_str());
		return -1;
	}

	this->mHasNextInstance = true;
	this->mNextInstance = new DenseInstance();
	this->mNextInstance->saveInstanceInformation(this->mInstanceInformation);
	this->mNextInstance->addLabels(labels);
	this->mNextInstance->addValues(values);

	return 0;
}

bool ArffReader::openFile(const string& fileName) {
	mFile->open(fileName.c_str());
	if (!mFile->is_open()) {
		LOG_ERROR("Failed to open file: %s .", fileName.c_str());
		return false;
	}
	return true;
}

