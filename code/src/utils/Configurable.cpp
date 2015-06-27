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

#include "Configurable.h"
#include "../Common.h"

Configurable::Configurable() {
	mStoredPraramValues = new Json::Value;
};

Configurable::~Configurable() {
	delete mStoredPraramValues;
};

bool Configurable::setParams(const string& params) {
   Json::Reader reader;
   bool ret = reader.parse( params, *mStoredPraramValues );
   if (ret) {
	   doSetParams();
	   return true;
   }
   else {
	   return false;
   }
}

int  Configurable::getParam(const string& paramName, const int& defaultValue) {
	return mStoredPraramValues->get(paramName, defaultValue).asInt();
}

double  Configurable::getParam(const string& paramName, const double& defaultValue) {
	return mStoredPraramValues->get(paramName, defaultValue).asDouble();
}

bool  Configurable::getParam(const string& paramName, const bool& defaultValue) {
	return mStoredPraramValues->get(paramName, defaultValue).asBool();
}

const string  Configurable::getParam(const string& paramName, const string& defaultValue) {
	return mStoredPraramValues->get(paramName, defaultValue).asString().c_str();
}

const string Configurable::getParam(const string& paramName, const char* defaultValue) {
	return mStoredPraramValues->get(paramName, defaultValue).asString().c_str();
}

const string Configurable::exportParams() {
	// if you want export your class's parameters, as following method:
	//Json::Value root;
	//root["your parameters 1"] = parameterValue1;
	//root["your parameters 2"] = parameterValue2;
	//...
	//return root.toStyledString().c_str();

	return "";
}

void Configurable::doSetParams() {
}

Json::Value Configurable::getParam(const string& paramName) {
	return (*mStoredPraramValues)[paramName.c_str()];
}
