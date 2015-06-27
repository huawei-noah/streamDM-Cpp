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


/**
 * CommandLineParser.h
 *
 *
 *  Brief class CommandLineParser is used for parser the command line.
 *  command line style like:
 *  1. smartDM experiment.json
 *  2. smartDM "EvaluatePrequential -l (Bagging -l (VfdtLearner) -s 100) -f 1000 -l 1000"
 *  The output data is a struct within the some parameter strings.
 */

#ifndef COMMANDLINEPARSER_H_
#define COMMANDLINEPARSER_H_

#include <string>
#include <map>

#include "json.h"
using namespace std;


/**
 * Command Line Parameter
 *
 * data:
 *
 *
 *
 *
 */
class CommandLineParameter {
public:
	CommandLineParameter();
	~CommandLineParameter();
public:
	string getTaskName();
	string getTaskParameter();
private:
	Json::Value data;
};

/**
 * Parameter transfer
 * CLPFN means: Command Line Parameter Full Names
 * for example: transfer "-l" to "learner"
 */
class CLPFN  {
private:
	CLPFN();
	CLPFN(const CLPFN&);
	CLPFN & operator = (const CLPFN &);
public:
	static CLPFN& getInstance();
public:
	/**
	 * store all parameter full name
	 * for example:
	 * 		data["EvaluatePrequential"]["type"] = "Task";
	 * 		data["EvaluatePrequential"]["-l"] = "Learner";
	 * 		data["VfdtLearner"]["type"] = "Learner";
	 * 		data["VfdtLearner"]["-sc"] = "SplitConfidence";
	 */
	map<string, map< string, string> > data;
};

class RegisterCommandLineParameterFullName {
public:
	RegisterCommandLineParameterFullName(const string& fullName);
};

#define REGISTER_COMMAND_LINE_PARAMETER(className, parameter) \
	RegisterCommandLineParameterFullName __CLPFN_##className##__(parameter) ;

class CommandLineParser {
public:
	static bool parser(int argc, char* argv[], string& taskName, string& taskParam);
private:
	CommandLineParser();
	virtual ~CommandLineParser();
	static bool parserCommandLine(const string& in, string& taskName, string& taskParam);
	static bool parserJsonFile(const string& in, string& taskName, string& taskParam);
	static bool parser(vector<string>& vec, const string& type, int& pos, Json::Value& jv);
	static string parser(const string& in);
};

#endif /* COMMANDLINEPARSER_H_ */
