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

#ifndef INSTANCECONDITIONALTEST_H_
#define INSTANCECONDITIONALTEST_H_

#include <vector>
#include "../../../Common.h"
using namespace std;

namespace HT {

enum InstanceConditionalTestType {
	T_InstanceConditionalTest,
	T_InstanceConditionalBinaryTest,
	T_NumericAttributeBinaryTest,
	T_NominalAttributeMultiwayTest,
	T_NominalAttributeBinaryTest
};

class InstanceConditionalTest : public RTTI {
public:
	InstanceConditionalTest();
	virtual ~InstanceConditionalTest();
	virtual int branchForInstance(const Instance* inst) = 0;
	virtual bool resultKnownForInstance(const Instance* inst);
	virtual int maxBranches() = 0;
	virtual vector<int>* getAttsTestDependsOn() = 0;
	virtual InstanceConditionalTest* clone() = 0;
	virtual void toJson(Json::Value& jv) = 0;
	virtual int getAttIndex() = 0;
	virtual double getAttValue() = 0;
};

class InstanceConditionalBinaryTest: public InstanceConditionalTest {
public:
	virtual int maxBranches();
};

class NumericAttributeBinaryTest: public InstanceConditionalBinaryTest {
public:
	int attIndex;
	double attValue;
	bool equalsPassesTest;
	NumericAttributeBinaryTest(int attIndex, double attValue,
			bool equalsPassesTest);
	NumericAttributeBinaryTest(const Json::Value& jv);
	int branchForInstance(const Instance* inst);
	vector<int>* getAttsTestDependsOn();
	double getSplitValue();
	virtual InstanceConditionalTest* clone();
	virtual void toJson(Json::Value& jv);
	virtual int getAttIndex();
	virtual double getAttValue();
};

class NominalAttributeMultiwayTest: public InstanceConditionalTest {
public:
	int attIndex;
	NominalAttributeMultiwayTest(int attIndex);
	NominalAttributeMultiwayTest(const Json::Value& jv);
	int branchForInstance(const Instance* inst);
	int maxBranches();
	vector<int>* getAttsTestDependsOn();
	virtual InstanceConditionalTest* clone();
	virtual void toJson(Json::Value& jv);
	virtual int getAttIndex();
	virtual double getAttValue();
};

class NominalAttributeBinaryTest: public InstanceConditionalBinaryTest {
public:
	int attIndex;
	int attValue;
	NominalAttributeBinaryTest(int attIndex, int attValue);
	NominalAttributeBinaryTest(const Json::Value& jv);
	int branchForInstance(const Instance* inst);
	vector<int>* getAttsTestDependsOn();
	virtual InstanceConditionalTest* clone();
	virtual void toJson(Json::Value& jv);
	virtual int getAttIndex();
	virtual double getAttValue();
};

} /* namespace HT */

#endif /* INSTANCECONDITIONALTEST_H_ */
