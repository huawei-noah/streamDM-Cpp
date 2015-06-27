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


#include "CommandLineParser.h"
#include "../Common.h"


////////////////////////////// CommandLineParser //////////////////////////////


CommandLineParser::CommandLineParser() {
}

CommandLineParser::~CommandLineParser() {
}

/**
 * parser command line string:
 * 1. smartdm -f file.json
 * 2. smartdm "EvaluateHoldOut -l (StreamingMBT) -r (StreamingMBTReader) -a train.txt -t test.txt"
 * \param argc command line parameter number
 * \param argv command line parameter
 * \param param the output data
 */
bool CommandLineParser::parser(int argc, char* argv[], string& taskName, string& taskParam) {

	if (argc != 2 && argc != 3) {
		LOG_ERROR("Command line arguments error.");
		return false;
	}

	if (argc == 2) {
		return parserCommandLine(argv[1], taskName, taskParam);
	}

	if (argc == 3) {
		string param(argv[1]);
		string value(argv[2]);
		if (param != "-f") {
			LOG_ERROR("Command option: smartdm -f file.json");
			return false;
		}else if (! Utils::checkFileExist(value)) {
			LOG_ERROR("File is not existed. %s", value.c_str());
			return false;
		}else{
			return parserJsonFile(argv[1], taskName, taskParam);
		}
	}
	return true;
}

/**
 * parser command line string,
 * link:
 * 1. smartDM "EvaluatePrequential -l (VfdtLearner -sc 0.001 -tc 0.02) -r C45Reader -ds /opt/moa/alan/test/data/DF"
 * 2. smartDM "EvaluatePrequential -l (Bagging -l (VfdtLearner -sc 0.001 -tc 0.02) -es 100) -r C45Reader -ds /opt/moa/alan/test/data/DF"
 * 3. smartDM "EvaluatePrequential -l (VfdtLearner -sc 0.001 -tc 0.02) -r (C45Reader -ds /opt/moa/alan/test/data/DF)"
 */
bool CommandLineParser::parserCommandLine(
		const string& in, string& taskName, string& taskParam) {

	// segment "(", ")" with " "
	stringstream ss;
	for (int i=0; i < in.size(); i++) {
		if (in[i] == '(' || in[i] == ')') {
			ss << " " << in[i] << " ";
		}
		else {
			ss << in[i];
		}
	}
	string s;
	vector<string> vec;
	while (getline(ss, s, ' ' )) {
		if (s.size() == 0) {
			continue;
		}
		vec.push_back(s);
	}

	Json::Value jv;
	int pos = 0;
	string type = "Task";
	bool ret = parser(vec, type, pos, jv);
	if (!ret) {
		return false;
	}

	taskName = jv["Name"].asString();
	taskParam = jv.toStyledString();

	return true;
}

/**
 * parser json file,
 * \param in is json file name.
 *
 * The file content model is :
 *
{
    "Task": {
        "Name": "EvaluatePrequential",
        "-l": {
            "Name": "VfdtLearner",
            "-sc": "0.001",
            "-tc": "0.02"
        },
        "-r": "C45",
        "-ds": "/opt/moa/alan/test/data/DF"
    }
}
 *
{
    "Task": {
        "Name": "EvaluatePrequential",
        "-l": {
            "Name": "Bagging",
            "-l": {
                "-sc": "0.001",
                "-tc": "0.02",
                "Name": "VfdtLearner"
            }
        },
        "-r": "C45",
        "-ds": "/opt/moa/alan/test/data/DF"
    }
}
 *
 */
bool CommandLineParser::parserJsonFile(
		const string& in, string& taskName, string& taskParam) {


	return true;
}

bool CommandLineParser::parser(vector<string>& vec, const string& type,
		int& pos, Json::Value& jv) {
	CLPFN &names = CLPFN::getInstance();

	string className = vec[pos];
	jv["Name"] = className;
	if (names.data.find(className) == names.data.end()) {
		LOG_ERROR("Not defined class: %s .", className.c_str());
		return false;
	}
	auto &classParams = names.data[className];

	pos++;
	while (pos < vec.size()) {
		// check exit condition
		if (vec[pos] == ")") {
			pos++;
			return true;
		}

		// get name
		string name = vec[pos];
		if (name[0] != '-') {
			LOG_ERROR("Error command line parameter: %s .", name.c_str());
			return false;
		}
		auto iter = classParams.find(name);
		if (iter == classParams.end()) {
			LOG_ERROR("Not define class parameter, class: %s, parameter: %s .",
					className.c_str(), name.c_str());
			return false;
		}
		name = iter->second;

		// get value
		pos++;
		if (pos == vec.size()) {
			LOG_ERROR("Require command line parameter value: %s .", name.c_str());
			return false;
		}
		string value = vec[pos];

		// check nestling
		if (value == "(") {
			Json::Value jv2;
			pos++;
			if (pos+1 == vec.size() ) {
				LOG_ERROR("Not enough command line parameter.");
				return false;
			}
			bool ret = parser(vec, name, pos, jv2 );
			if (!ret) {
				return false;
			}
			jv[name] = jv2;
		}
		else {
			jv[name] = value;
			pos++;
		}
	}

	return true;
}

////////////////////////////// CommandLineParameter ///////////////////////////


CommandLineParameter::CommandLineParameter() {
}

CommandLineParameter::~CommandLineParameter() {
}

string CommandLineParameter::getTaskName() {
	return data["Task"]["Name"].asString();
}

string CommandLineParameter::getTaskParameter() {
	return data["Task"].toStyledString();
}

////////////////////////////// CLPFN //////////////////////////////////////////

CLPFN::CLPFN() {
}

CLPFN& CLPFN::getInstance() {
	static CLPFN instance;
	return instance;
}

RegisterCommandLineParameterFullName::RegisterCommandLineParameterFullName(
		const string& fullName) {
	Json::Value jv;
	Json::Reader reader;
	reader.parse( fullName, jv );
	CLPFN& names = CLPFN::getInstance();

	string name = jv["name"].asString();
	string type = jv["type"].asString();
	Json::Value::Members m = jv["parameter"].getMemberNames();
	names.data[name]["Type"] = type;
	for (int i=0; i<m.size(); i++) {
		names.data[name][m[i]] = jv["parameter"][m[i]].asString();
	}
}

