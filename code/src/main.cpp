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

#include "Common.h"
#include "tasks/Task.h"
using namespace std;

int main(int argc, char* argv[]) {
	const std::string initFileName = "conf/log4cpp.properties";

	Log::openLog(initFileName);

	string taskName;
	string taskParams;
	bool ret = CommandLineParser::parser(argc, argv, taskName, taskParams);
	if (!ret) {
		return 0;
	}

	Task* task = (Task*) CREATE_CLASS(taskName);
	task->setParams(taskParams);
	task->doTask();

	Log::closeLog();
	return 0;
}
