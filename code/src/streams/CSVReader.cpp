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


/*
 * CSVReader.cpp
 * current CSVReader is suitable for the data with same attribute features,
 * not suitable for data with both numeric attributes and nominal attributes
 */

#include "CSVReader.h"
#include "../core/DenseInstance.h"
#include "../Common.h"

REGISTER_CLASS(CSVReader) ;
REGISTER_COMMAND_LINE_PARAMETER(
		CSVReader,
		"{\"type\":\"Reader\","
		"\"name\":\"CSVReader\","
		"\"parameter\":{"
		"\"-n\":\"AttributesNum\","
		"\"-nan\":\"NegativeOneAsNone\","
		"\"-cls\":\"classes\","
		"\"-deli\":\"delimiter\","
		"\"-v\":\"value\","
		"\"-aid\":\"attrid\","
		"\"-fl\":\"firstLine\""
		"}}"
		"");

CSVReader::CSVReader():
mDataFile(nullptr),
mNumAttributes(0),
mInstanceInformation(nullptr),
mFisrtLine(false),
mDelimiter(','){
	this->mNegativeOneAsNone = false;
}

CSVReader::~CSVReader() {
	if (mDataFile != nullptr) {
		if (mDataFile->is_open()) {
			mDataFile->close();
		}
		delete mDataFile;
	}

	if (mInstanceInformation != nullptr) {
		delete mInstanceInformation;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CSVReader::doSetParams() {
	this->mNegativeOneAsNone = (getParam("NegativeOneAsNone", 0) == 1);
	this->mInstanceInformation = new InstanceInformation(this->mNegativeOneAsNone);
	mNumAttributes = getParam("AttributesNum", 54);

	vector < string > attrValueVec;
	Json::Reader reader;
	Json::Value jv;

	string attrValueList = getParam("value", "");
	if (!attrValueList.empty()) {
		bool ret = reader.parse( attrValueList, jv );
		if (!ret) {
			LOG_ERROR("CSV Reader file attribute value configuration error!");
			return;
		}

		attrValueVec.resize(jv.size());
		for (unsigned int j = 0; j < jv.size(); ++j) {
			attrValueVec[j] = jv[j].asString();
		}
	}

	set<int> setAttribID;
	string attridList = getParam("attrid", "");
	if (!attridList.empty()) {
		bool ret = reader.parse( attridList, jv );
		if (!ret) {
			LOG_ERROR("CSV Reader file attribute id configuration error!");
			return;
		}

		for (unsigned int j = 0; j < jv.size(); ++j) {
			setAttribID.insert(atoi(jv[j].asString().c_str()));
		}
	}

	for (int i=0; i<mNumAttributes; ++i) {
		if ((setAttribID.end() != setAttribID.find(i))) {
			this->mInstanceInformation->addAttribute(new Attribute(attrValueVec), i);
		}
		else {
			this->mInstanceInformation->addAttribute(new Attribute(), i);
		}
	}

	vector < string > vals;
	string clsList = getParam("classes").asString();
	bool ret = reader.parse( clsList, jv );
	if (!ret) {
		LOG_ERROR("CSV Reader file classes number configuration error!");
		return;
	}

	vals.resize(jv.size());
	for (unsigned int j = 0; j < jv.size(); ++j) {
		vals[j] = jv[j].asString();
	}

	this->mInstanceInformation->addClass(new Attribute(vals), 0);

	mFisrtLine = getParam("firstLine", false);

	string strTmp = getParam("delimiter", ",");
	mDelimiter = strTmp[0];

	LOG_DEBUG("CSVReader numAttr=%d, cls=%d, first=%d, deli=%c",
			mNumAttributes, vals.size(), mFisrtLine, mDelimiter);
}

int CSVReader::input(string& instance) {
	string number;
	istringstream csvStream(instance);
	int i = 0;
	vector<double> labels(1);
	vector<double> values(mNumAttributes);

	// init label to NAN, if instance has no label, getLabel() get NAN
	labels[0] = NAN;
	while (getline(csvStream, number, mDelimiter)) {
		if (i == mNumAttributes) {
			labels[0] = this->mInstanceInformation->getOutputAttributeIndex(0, number);
			break;
		}
		else {
			values[i] = this->mInstanceInformation->getAttributeInternalValue(i, number);
		}
//		inst->addValue(value, i);
		i++;
	}

	this->mHasNextInstance = true;
	Instance* inst = new DenseInstance();
	this->mNextInstance = inst;
	this->mNextInstance->setInstanceInformation(this->mInstanceInformation);
	this->mNextInstance->addLabels(labels);
	this->mNextInstance->addValues(values);

	return 0;
}

int CSVReader::inputForDynamicAttributes(string& instance, bool bwithlabel) {
	string number;
	istringstream csvStream(instance);

	vector<string> line;

	if (bwithlabel) {
		line.reserve(mNumAttributes+1);

		while (getline(csvStream, number, mDelimiter)) {
			line.push_back(number);
		}

		if (line.size() != (unsigned int)(mNumAttributes+1)) {
			vector<string> vecValue;
			this->mInstanceInformation->setNumberInputAttributes(line.size()-1, vecValue);
			mNumAttributes = line.size()-1;
		}
	}
	else {
		line.reserve(mNumAttributes);

		while (getline(csvStream, number, mDelimiter)) {
			line.push_back(number);
		}

		if (line.size() != (unsigned int)mNumAttributes) {
			vector<string> vecValue;
			this->mInstanceInformation->setNumberInputAttributes(line.size(), vecValue);
			mNumAttributes = line.size();
		}
	}

	vector<double> labels(1);
	vector<double> values(mNumAttributes);

	// init label to NAN, if instance has no label, getLabel() get NAN
	labels[0] = NAN;
	for (unsigned int i = 0; i < line.size(); ++i) {
		if (i == (unsigned int)mNumAttributes) {
			labels[0] = this->mInstanceInformation->getOutputAttributeIndex(0, line[i]);
			break;
		}
		else {
			values[i] = this->mInstanceInformation->getAttributeInternalValue(i, line[i]);
		}
	}

	this->mHasNextInstance = true;
	Instance* inst = new DenseInstance();
	this->mNextInstance = inst;
	this->mNextInstance->setInstanceInformation(this->mInstanceInformation);
	this->mNextInstance->addLabels(labels);
	this->mNextInstance->addValues(values);

	return 0;
}

bool CSVReader::setFile(const string& fileName) {
	//this->mDataFile = new ifstream();
	//mDataFile->open(fileName.c_str());
	this->mDataFile = new fstream(fileName.c_str(), ios::in);
	if (!mDataFile->is_open()) {
		//LOG_ERROR("Failed to open file: %s .", fileName.c_str());
		return false;
	}

	if (mFisrtLine) {
		string line;
		if (getline(*mDataFile, line)) {
			LOG_INFO("first line of file is attrib name line.");
		}
		else {
			return false;
		}
	}

	return true;
}

bool CSVReader::resetFile(const string& fileName)
{
	if (mDataFile != nullptr) {
		if (mDataFile->is_open()) {
			mDataFile->close();
		}
		delete mDataFile;
	}

	return this->setFile(fileName);
}

bool CSVReader::hasNextInstance() {
	string line;
	if (getline(*mDataFile, line)) {
		input(line);
	} else {
		mHasNextInstance = false;
	}
	return mHasNextInstance;
}
