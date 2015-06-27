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

#ifndef CSVREADER_H_
#define CSVREADER_H_

#include "Reader.h"

class CSVReader: public Reader {
public:
	CSVReader();
	virtual ~CSVReader();
	virtual int input(string&);
	virtual bool setFile(const string&);
	virtual bool resetFile(const string&);
	virtual bool hasNextInstance();
	virtual int inputForDynamicAttributes(string& instance, bool bwithlabel = false);

protected:
	InstanceInformation* mInstanceInformation;
	std::fstream* mDataFile;
private:
	void doSetParams();
	//void setDefault();
	int mNumAttributes;
	bool mFisrtLine; //	true: first line of file is attrib name line, false: data line
	char mDelimiter; // delimiter of csv file data
	bool mNegativeOneAsNone;
};

#endif /* CSVREADER_H_ */
