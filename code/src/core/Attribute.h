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

#ifndef ATTRIBUTE_H_
#define ATTRIBUTE_H_
#include <string>
#include <vector>
#include "../utils/json.h"
using namespace std;

class Attribute {
public:
	Attribute();
	Attribute(const vector<string>& values);
	virtual ~Attribute();
	bool isNumeric();
	bool isNominal();
	int numberValues();
	string getValue(int);
	Attribute* clone();

	Json::Value toJson();

protected:
	bool mIsNumeric;
	vector<string> mValues;
};

#endif /* ATTRIBUTE_H_ */
