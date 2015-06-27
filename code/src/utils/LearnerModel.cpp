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

#include "LearnerModel.h"
#include "Log.h"
#include "Utils.h"
#include <fstream>
#include <sstream>
#include <iostream>

LearnerModel::LearnerModel() {
}

LearnerModel::~LearnerModel() {
}

bool LearnerModel::exportToFile(const string& fileName) {
	Json::Value jv;

	if (!exportToJson(jv)) {
		return false;
	}

	return saveJsonToFile(fileName, jv);
}

bool LearnerModel::importFromFile(const string& fileName) {
	if (!Utils::checkFileExist(fileName)) {
		LOG_ERROR("File not existed. %s", fileName.c_str());
		return false;
	}

	Json::Value jv;
	if (! loadJsonFromFile(fileName, jv)) {
		return false;
	}

	return importFromJson(jv);
}

bool LearnerModel::saveJsonToFile(const string& fileName, Json::Value& jv) {
	ofstream ofs(fileName);

	if (!ofs.is_open()) {
		LOG_ERROR("Can not create file: %s", fileName.c_str());
		return false;
	}

	Json::StyledStreamWriter writer;
	writer.write(ofs, jv);
	ofs.close();

//	Json::FastWriter writer;
//	ofs << writer.write(jv);
//	ofs.close();

	return true;
}

bool LearnerModel::exportToJson(Json::Value& jv) {
	// need implement
	return true;
}

bool LearnerModel::importFromJson(const Json::Value& jv) {
	return true;
}

bool LearnerModel::loadJsonFromFile(const string& fileName, Json::Value& jv) {
	ifstream ifs(fileName);
	if (!ifs.is_open()) {
		LOG_ERROR("Can not read from file: %s", fileName.c_str());
		return false;
	}

	Json::Reader reader;
	if (!reader.parse(ifs, jv)) {
		LOG_ERROR("File format error: %s", fileName.c_str());
		ifs.close();
		return false;
	}
	ifs.close();

	return true;
}


