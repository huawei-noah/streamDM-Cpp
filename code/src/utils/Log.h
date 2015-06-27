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

#ifndef Log_H_
#define Log_H_

#ifdef _MSC_VER
#pragma comment(lib, "log4cpp.lib")  
#pragma comment(lib, "ws2_32.lib")  
#endif

#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>

#ifdef _MSC_VER
#define LOG_DEBUG ::log_smartDM.debug
#define LOG_INFO ::log_smartDM.info
#define LOG_WARN ::log_smartDM.warn
#define LOG_ERROR ::log_smartDM.error
#else
#define LOG_DEBUG(x, args...) log_smartDM.debug(x, ##args)
#define LOG_INFO(x, args...) log_smartDM.info(x, ##args)
#define LOG_WARN(x, args...) log_smartDM.warn(x, ##args)
#define LOG_ERROR(x, args...) log_smartDM.error(x, ##args)
#endif

extern log4cpp::Category& log_smartDM;

class Log {
public:
	static bool openLog(const std::string& configfile);
	static void closeLog();
};

#endif /* Log_H_ */
