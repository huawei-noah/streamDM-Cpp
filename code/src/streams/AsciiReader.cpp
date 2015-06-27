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

#include "AsciiReader.h"

REGISTER_CLASS(AsciiReader) ;

AsciiReader::AsciiReader() {
	this->mInstanceInformation = new InstanceInformation();
	this->mFile = new ifstream();
	this->mFileOpened = false;
}

AsciiReader::~AsciiReader() {
	renew();
	delete mInstanceInformation;
	delete mFile;
}


Instance* AsciiReader::nextInstance() {
	if (! this->hasNextInstance()) {
		return NULL;
	}

	Instance* ret = mNextInstance;
	readData();

	return ret;
}

bool AsciiReader::setFile(const string& fileName) {

	renew();

	if (! checkFileExist(fileName)) {
		return false;
	}
	
	if (! openFile(fileName)) {
		return false;
	}	

	// read first row
	if (! readData()) {
		return false;
	}

	return true;
}

void AsciiReader::renew() {
	if (mFileOpened) {
		if (mFile->is_open()) {
			mFile->close();
		}

		delete mInstanceInformation;
		mInstanceInformation = new InstanceInformation();
		if (this->mHasNextInstance) {
			delete this->mNextInstance;
		}
	}

	this->mHasNextInstance = false;
	mFileOpened = false;
}

bool AsciiReader::checkFileExist(const string& fileName) {
	if (access(fileName.c_str(), 0) != 0 ) {
		return false;
	}
	return true;
}


bool AsciiReader::readData() {

	mHasNextInstance = false;
	string s = "";

	// continue read next line when current line's data is wrong
	while (getline(*mFile,s)) {
		if (s.size() != 0 ) {
			if (input(s) == 0) {
				return true;
			}
		}
	}

	renew();

	return true;
}

int AsciiReader::input(string& s) {

	int index = 0;
	string item = "";
	double d = 0;
	this->mHasNextInstance = false;

	stringstream ss(s);
	int first = 0, second = 0, third = 0;
	vector<double> values;
	while(getline(ss, item, ' ')) {
		sscanf(item.c_str(), "%lf", &d);
		values.push_back(d);
		switch (index)
		{
		case 0: 
			first = (int)d;
			break;
		case 1:
			second = (int)d;
			break;
		case 2:
			third = (int)d;
			break;
		default:
			break;
		}

		index++;
	}

	// valid data
	if (first != second || third != index-3) {
		return -1;
	}

	this->mHasNextInstance = true;
	this->mNextInstance = new DenseInstance();
	this->mNextInstance->addValues(values);

	return 0;
}

bool AsciiReader::openFile(const string& fileName) {
	mFile->open(fileName.c_str());
	if (! mFile->is_open()) {
		LOG_ERROR("Failed to open file: %s .", fileName.c_str());
		return false;
	}
	return true;
}



