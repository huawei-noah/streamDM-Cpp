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

#ifndef DYNAMICCREATECLASS_H_
#define DYNAMICCREATECLASS_H_

#include <string>
#include <map>
#include <vector>
using namespace std;

typedef void* (*__DynamicCreateFuntion)(void);  
      
class DynamicCreateClassFactory  {
private:
	DynamicCreateClassFactory();
	DynamicCreateClassFactory(const DynamicCreateClassFactory &);
	DynamicCreateClassFactory & operator = (const DynamicCreateClassFactory &);
public:  
	static DynamicCreateClassFactory& getInstance();
	void* getClassByName(std::string name);
	void registerClass(std::string name,__DynamicCreateFuntion method);
	std::map<std::string, __DynamicCreateFuntion> mClassMap;
};


template<class T, const char name[]>  
class DynamicRegisterClass {
public:
	DynamicRegisterClass(){
		DynamicCreateClassFactory::getInstance().registerClass(name, DynamicRegisterClass::createInstance);
	};
	~DynamicRegisterClass(){};
	static void* createInstance() {
		return new T();
	};
};


#define REGISTER_CLASS(class_name) \
	char __##class_name##_Array[]=#class_name;\
	DynamicRegisterClass<class_name, __##class_name##_Array> __drc_##class_name##__ ;

#define CREATE_CLASS(class_name) \
	DynamicCreateClassFactory::getInstance().getClassByName(class_name)

#endif /* DYNAMICCREATECLASS_H_ */
