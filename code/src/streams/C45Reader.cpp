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

#include "C45Reader.h"

REGISTER_CLASS(C45Reader)
REGISTER_COMMAND_LINE_PARAMETER(C45Reader, "{\"type\":\"Reader\","
		"\"name\":\"C45Reader\","
		"\"parameter\":{"
		"\"-d\":\"DynamicAttributes\""
		"}}"
		"");

C45Reader::C45Reader() {
	this->mInstanceInformation = new InstanceInformation();
	this->mDataFile = new ifstream();
	this->mAttributes = NULL;
	this->mDynamicAttributes = false;
	this->mNamesLoaded = false;
}

C45Reader::~C45Reader() {
	renew();
	delete mInstanceInformation;
	delete mDataFile;
}

void C45Reader::doSetParams() {
	this->mDynamicAttributes = (this->getParam("DynamicAttributes", 0) == 1);
}

C45Reader::LocalAttributes::LocalAttributes(InstanceInformation* ii) {
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

double C45Reader::LocalAttributes::getIndex(const int& sequence,
		const string& value) {
	if (value == "?") {
		return NAN;
	}
	if (this->isNumeric[sequence]) {
		return atof(value.c_str());
	} else {
		// if the value is not existed in this->values,
		// maybe save it in this->values.
		return this->values[sequence][value];
	}
}

Instance* C45Reader::nextInstance() {
	if (!this->hasNextInstance()) {
		return NULL;
	}

	Instance* ret = mNextInstance;
	readData();

	return ret;
}

bool C45Reader::setFile(const string& fileName) {

	renew();

	if (!checkFileExist(fileName)) {
		return false;
	}

	if (readAllDataFromNamesFile(fileName + ".names") != 0) {
		return false;
	}

	mNamesLoaded = true;

	string f = fileName + ".data";
	if (openDataFile(f) != 0) {
		return false;
	}

	// read first row
	if (readData() != 0) {
		return false;
	}

	return true;
}

void C45Reader::renew() {

	if (mNamesLoaded) {
		if (mDataFile->is_open()) {
			mDataFile->close();
		}

		delete mInstanceInformation;
		mInstanceInformation = new InstanceInformation();

		if (this->mHasNextInstance) {
			delete this->mNextInstance;
		}
	}

	if (this->mAttributes != NULL) {
		delete this->mAttributes;
		this->mAttributes = NULL;
	}

	this->mHasNextInstance = false;
	mNamesLoaded = false;
}

bool C45Reader::checkFileExist(const string& fileName) {
	string namesFile = fileName + ".names";
	string dataFile = fileName + ".data";

	if (access(namesFile.c_str(), 0) != 0) {
		return false;
	}

	if (access(dataFile.c_str(), 0) != 0) {
		return false;
	}

	return true;
}

int C45Reader::trimNamesRowData(string& data) {
	int pos = 0;
	string temp = "";

	if (data.length() == 0) {
		return 0;
	}

	// remove comment data. ('|');
	pos = data.find_first_of('|');
	if (pos >= 0) {
		data = data.substr(0, pos);
	}

	// remove header and tailer charactor: " "
	data.erase(0, data.find_first_not_of(" \r\n\t"));
	data.erase(data.find_last_not_of(" \r\n\t") + 1);

	// remove "."
	if (data[data.length() - 1] == '.') {
		data = data.substr(0, data.length() - 1);
	}

	// check data length
	if (data.length() == 0) {
		return 0;
	}
	if (data.length() <= 2) {
		LOG_ERROR(".names file has not enough data.");
		return -1;
	}

	// convert "\t" to " "
	temp = "";
	for (size_t i = 0; i < data.length(); i++) {
		if (data[i] == '\t') {
			temp += " ";
		} else {
			temp += data[i];
		}
	}
	data = temp.c_str();

	// remove double white space   
	// "  " -> " "
	temp = "";
	temp += data[0];
	for (size_t i = 1; i < data.length(); i++) {
		if (data[i] == ' ' && data[i - 1] == ' ') {
			continue;
		} else {
			temp += data[i];
		}
	}
	data = temp.c_str();
	if (data.length() <= 2) {
		// data is empty
		return 0;
	}

	// remove the white space
	// ", " -> ","
	// " ," -> ","
	// ": " -> ":"
	// " :" -> ":"
	temp = "";
	temp += data[0];
	for (size_t i = 1; i < data.length() - 1; i++) {
		if (data[i] == ' ') {
			if (data[i - 1] == ',' || data[i - 1] == ':' || data[i + 1] == ','
					|| data[i + 1] == ':') {
				continue;
			} else {
				temp += data[i];
			}
		} else {
			temp += data[i];
		}
	}
	temp += data[data.length() - 1];
	data = temp.c_str();

	return 0;
}

int C45Reader::loadClassNames(const string& data, const int index) {
	string s = data.c_str();
	int last = 0;
	vector<string> vec;

	// split data
	for (size_t i = 0; i < s.length(); i++) {
		if (s[i] == ',') {
			vec.push_back(s.substr(last, i - last));
			last = i + 1;
		}
	}
	vec.push_back(s.substr(last, s.length() - last));

	Attribute* att = new Attribute(vec);
	this->mInstanceInformation->addClass(att, 0);

	return 0;
}

int C45Reader::loadAttributes(const string& data, const int index) {
	string s = data.c_str();
	int last = 0;

	int pos = s.find(":");
	if (pos < 0) {
		LOG_ERROR("The .names data format error(%s). ", data.c_str());
		return -1;
	}

	if (s[0] == ':' || s[0] == ',' || s[s.length() - 1] == ':'
			|| s[s.length() - 1] == ',') {
		LOG_ERROR("The .names data format error(%s).", data.c_str());
		return -1;
	}

	vector<string> vec;
	last = pos + 1;

	// split data
	string item;
	int k = 0;
	for (size_t i = pos + 1; i < s.length(); i++) {
		if (s[i] == ',') {
			item = s.substr(last, i - last).c_str();
			if (k == 0 && item == "continuous") {
				this->mInstanceInformation->addAttribute(new Attribute(),
						index);
				return 0;
			}
			vec.push_back(item);
			last = i + 1;
			k++;
		}
	}
	item = s.substr(last, s.length() - last).c_str();
	if (k == 0 && item == "continuous") {
		this->mInstanceInformation->addAttribute(new Attribute(), index);
		return 0;
	}
	vec.push_back(item);

	this->mInstanceInformation->addAttribute(new Attribute(vec), index);

	return 0;
}

int C45Reader::readData() {

	if (!mNamesLoaded) {
		LOG_ERROR("You must call setFile() function before read data.");
		return -1;
	}

	mHasNextInstance = false;
	string s = "";

	if (!getline(*mDataFile, s)) {
		return 0;
	}

	if (this->mDynamicAttributes) {
		return this->inputForDynamicAttributes(s);
	} else {
		return this->input(s);
	}
}

int C45Reader::input(string& s) {

	int index = 0;
	string item = "";
	double d = 0;
	this->mHasNextInstance = false;

	stringstream ss(s);
	vector<double> labels(1);
	vector<double> values(mAttributes->count - 1);
	while (getline(ss, item, ',')) {
		if (index >= this->mAttributes->count) {
			break;
		}
		// item.erase(0, item.find_first_not_of(" \r\n\t"));
		// item.erase(item.find_last_not_of(" \r\n\t") + 1);
		d = mAttributes->getIndex(index, item);
//		this->mNextInstance->addValue(d, index);
		if (index == this->mAttributes->count - 1) {
			labels[0] = d;
			break;
		} else {
			values[index] = d;
		}
		index++;
	}

	if (index + 1 != this->mAttributes->count) {
		LOG_ERROR("Data number error (%d).", index);
		return -1;
	}

	this->mHasNextInstance = true;
	this->mNextInstance = new DenseInstance();
	this->mNextInstance->setInstanceInformation(this->mInstanceInformation);
	this->mNextInstance->addLabels(labels);
	this->mNextInstance->addValues(values);

	return 0;
}

int C45Reader::inputForDynamicAttributes(string& s) {
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

int C45Reader::readAllDataFromNamesFile(const string& fileName) {

	ifstream f(fileName.c_str());
	if (!f.is_open()) {
		LOG_ERROR("Failed to open file: %s .", fileName.c_str());
		return -1;
	}

	// Read all data into memory
	string data;
	vector<string> allData;
	while (getline(f, data)) {
		data.erase(data.find_last_not_of(" \r\n\t") + 1);
		if (data.size() != 0) {
			allData.push_back(data);
		}
	}
	f.close();

	if (allData.size() <= 1) {
		LOG_ERROR(".names file(%s) is empty. ", fileName.c_str());
		return -1;
	}

	int index = 0;

	// deal with attribute
	for (size_t i = 1; i < allData.size(); i++) {
		data = allData[i];
		if (trimNamesRowData(data) < 0) {
			LOG_ERROR(".names data error(%s). ", data.c_str());
			return -1;
		}
		index = i - 1;
		if (loadAttributes(data, index) < 0) {
			LOG_ERROR(".names data error(%s). ", data.c_str());
			return -1;
		}
	}

	// deal with class names
	data = allData[0];
	if (trimNamesRowData(data) < 0) {
		LOG_ERROR(".names data error(%s). ", data.c_str());
		return -1;
	}
	index++;
	if (loadClassNames(data, index) < 0) {
		LOG_ERROR(".names data error(%s). ", data.c_str());
		return -1;
	}

	if (this->mAttributes != nullptr) {
		delete this->mAttributes;
		this->mAttributes = nullptr;
	}
	this->mAttributes = new LocalAttributes(mInstanceInformation);

	return 0;
}

int C45Reader::openDataFile(const string& fileName) {
	mDataFile->open(fileName.c_str());
	if (!mDataFile->is_open()) {
		LOG_ERROR("Failed to open file: %s .", fileName.c_str());
		return -1;
	}
	return 0;
}

int C45Reader::readDataFile(const string& fileName) {

	ifstream f(fileName.c_str());
	if (!f.is_open()) {
		LOG_ERROR("Failed to open file: %s . ", fileName.c_str());
		return -1;
	}

	// Read all data into memory.
	// If the real data file is very large, change the read method later.
	string data;
	vector<string> allData;
	while (getline(f, data)) {
		allData.push_back(data);
	}
	f.close();

	if (allData.size() <= 1) {
		LOG_WARN("Data file(%s) is empty.", fileName.c_str());
		return 0;
	}

	// deal with attribute
	for (size_t i = 0; i < allData.size(); i++) {
		data = allData[i];
		if (readData() < 0) {
			LOG_ERROR("Failed to read data from data file(%s).",
					fileName.c_str());
			return -1;
		}
	}

	return 0;
}

