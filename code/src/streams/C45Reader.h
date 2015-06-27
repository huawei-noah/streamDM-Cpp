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

#ifndef C45READER_H_
#define C45READER_H_

#include "../Common.h"

using namespace std;

class C45Reader: public Reader {
public:
	C45Reader();
	virtual ~C45Reader();
	virtual int input(string&);
	virtual int inputForDynamicAttributes(string&);
	virtual Instance* nextInstance();
	virtual bool setFile(const string& fileName);

protected:
	InstanceInformation* mInstanceInformation;
	virtual void doSetParams();
  
private:

	bool mNamesLoaded;
	ifstream* mDataFile;
	bool mDynamicAttributes;

	const static bool FLAG_SKIP_ERROR_ROWS = false;

	int trimNamesRowData(string& data);
	int loadClassNames(const string& data, const int index);
	int loadAttributes(const string& data, const int index);
	int readAllDataFromNamesFile(const string& fileName);

	int openDataFile(const string& fileName);
	int readData();

	int readDataFile(const string& fileName);
	void renew();

	bool checkFileExist(const string& fileName);

	struct LocalAttributes {
		int count;
		vector<bool> isNumeric;
		map<int, map<string, int> > values;
		LocalAttributes(InstanceInformation* ii);
		double getIndex(const int&,const string&);
	} *mAttributes;
};

#endif /* C45READER_H_ */

