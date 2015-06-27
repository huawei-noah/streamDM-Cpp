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

#ifndef LEARNERMODEL_H_
#define LEARNERMODEL_H_

#include <string>

#include "json.h"
using namespace std;

class LearnerModel {
public:
	LearnerModel();
	virtual ~LearnerModel();

	bool exportToFile(const string& fileName);
	bool importFromFile(const string& fileName);

public:
	virtual bool exportToJson(Json::Value& jv);
	virtual bool importFromJson(const Json::Value& jv);

private:
	bool saveJsonToFile(const string& fileName, Json::Value& jv);
	bool loadJsonFromFile(const string& fileName, Json::Value& jv);

};

#endif /* LEARNERMODEL_H_ */
