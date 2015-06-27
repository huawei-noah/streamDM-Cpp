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

#ifndef INSTANCE_INFORMATION_H
#define INSTANCE_INFORMATION_H
#include "Attribute.h"
#include <stdlib.h>
#include <map>
#include <string>
#include "../utils/json.h"
using namespace std;

class InstanceInformation {
public:
	InstanceInformation();
	InstanceInformation(const bool negativeOneAsNone);
	virtual ~InstanceInformation();
	int getNumberInputAttributes();
	int getNumberOutputAttributes();
	int getNumberClasses();
	Attribute* getInputAttribute(int);
	Attribute* getOutputAttribute(int);
	void addAttribute(Attribute*, int);
	void addClass(Attribute*, int);
	double getAttributeInternalValue(int, string);
	InstanceInformation* clone();

	// notes:only used in dynamic attributes scene
	void setNumberInputAttributes(int, vector<string>&);
	double getOutputAttributeIndex(int index, const string& strValue);

	Json::Value toJson();
private:
	map<int, Attribute*> mInputAttributes;
	map<int, Attribute*> mOutputAttributes;
	bool mNegativeOneAsNone;

};

#endif
