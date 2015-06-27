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

#ifndef ARFFREADER_H_
#define ARFFREADER_H_

#include "../Common.h"

using namespace std;

class ArffReader: public Reader {
public:
	ArffReader();
	virtual ~ArffReader();
	virtual int input(string&);
	virtual int inputForDynamicAttributes(string&);
	virtual Instance* nextInstance();
	virtual bool setFile(const string& fileName);

protected:
	InstanceInformation* mInstanceInformation;
	virtual void doSetParams();
  
private:

	bool mHeaderLoaded;
	ifstream* mFile;
	bool mDynamicAttributes;

	bool openFile(const string& fileName);
	bool readHeader();
	bool readData();
	bool parseHeaderData(const string& row);

	void renew();

	bool checkFileExist(const string& fileName);

	struct LocalAttributes {
		int count;
		vector<bool> isNumeric;
		map<int, map<string, int> > values;
		LocalAttributes();
		LocalAttributes(InstanceInformation* ii);
		void saveToInstanceInformation(InstanceInformation*& ii);
		void appendNumericAttribute();
		void appendDiscreteAttribute(const string& value);
		double getIndex(const int&,const string&);
	} *mAttributes;
};

#endif /* ARFFREADER_H_ */

