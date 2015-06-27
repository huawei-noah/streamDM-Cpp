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

#ifndef CONFIGURABLE_H_
#define CONFIGURABLE_H_

#include <string>

#include "json.h"
using namespace std;

class Configurable {
public:
	Configurable();
	~Configurable();
	virtual bool setParams(const string& params);
	int getParam(const string& paramName, const int& defaultValue);
	double getParam(const string& paramName, const double& defaultValue);
	bool getParam(const string& paramName, const bool& defaultValue);
	const string getParam(const string& paramName, const char* defaultValue);
	const string getParam(const string& paramName, const string& defaultValue);
	Json::Value getParam(const string& paramName);
	virtual const string exportParams();
protected:
	Json::Value* mStoredPraramValues;
	virtual void doSetParams();
};

#endif /* CONFIGURABLE_H_ */
