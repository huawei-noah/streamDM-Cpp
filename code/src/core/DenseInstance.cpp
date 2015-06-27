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

#include "DenseInstance.h"
#include <sstream>

DenseInstance::DenseInstance() {
	weight = 1;
}

DenseInstance::~DenseInstance() {
}

double DenseInstance::getInputAttributeValue(int index) const {
	return mInputData[index];
}

double DenseInstance::getOutputAttributeValue(int index) const {
	return mOutputData[index];
}

Json::Value DenseInstance::toJson() const {
	Json::Value jv;
	jv["class"] = mOutputData[0];
	for (int i=0; i<mInputData.size(); i++) {
		jv["data"][i] = mInputData[i];
	}
	return jv;
}

void DenseInstance::addValues(const vector<double>& values) {
	mInputData = values;
}

void DenseInstance::addLabels(const vector<double>& values) {
	mOutputData = values;
}

Instance* DenseInstance::clone(){
	DenseInstance* di = new DenseInstance();
	if (this->instanceInformationSaved) {
		di->instanceInformation = this->instanceInformation->clone();
	}else {
		di->instanceInformation = this->instanceInformation;
	}
	di->weight = this->weight;
	di->mInputData = this->mInputData;
	di->mOutputData = this->mOutputData;
	di->instanceInformationSaved = this->instanceInformationSaved;
	return di;
}

