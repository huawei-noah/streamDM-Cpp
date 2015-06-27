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

#include "DynamicCreateClass.h"
#include <iostream>

DynamicCreateClassFactory::DynamicCreateClassFactory() {
}

DynamicCreateClassFactory& DynamicCreateClassFactory::getInstance()
{
	static DynamicCreateClassFactory instance;
	return instance;
};

void* DynamicCreateClassFactory::getClassByName(std::string name){
	auto find =  DynamicCreateClassFactory::getInstance().mClassMap.find(name);
	if(find==mClassMap.end())  {
		// debug information
		cout << "not find class: " << name << endl;
		return nullptr;
	}
	else  {
		return find->second();
	}
}  
      

void DynamicCreateClassFactory::registerClass(std::string name, __DynamicCreateFuntion method)  {  
	DynamicCreateClassFactory::getInstance().mClassMap.insert(std::make_pair(name,method));
}  

