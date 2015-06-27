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

#include "InstanceInformation.h"
#include <iostream>
#include "../Common.h"

InstanceInformation::InstanceInformation() {
	this->mNegativeOneAsNone = false;
}

InstanceInformation::InstanceInformation(const bool negativeOneAsNone) {
	this->mNegativeOneAsNone = negativeOneAsNone;
}

InstanceInformation::~InstanceInformation() {
	for (auto iter = mInputAttributes.begin(); iter != mInputAttributes.end();
			iter++) {
		if (nullptr != iter->second) {
			delete iter->second;
		}
	}
	for (auto iter = mOutputAttributes.begin(); iter != mOutputAttributes.end();
			iter++) {
		if (nullptr != iter->second) {
			delete iter->second;
		}
	}
}

Json::Value InstanceInformation::toJson() {
	Json::Value jv;
	int size = (int)mInputAttributes.size();
	for (int i=0; i<size; i++) {
		jv["InputAttributes"][i] = mInputAttributes[i]->toJson();
	}
	size = (int)mOutputAttributes.size();
	for (int i=0; i<size; i++) {
		jv["OutputAttributes"][i] = mOutputAttributes[i]->toJson();
	}
	return jv;
}

int InstanceInformation::getNumberClasses() {
	return mOutputAttributes[0]->numberValues();
}

Attribute* InstanceInformation::getInputAttribute(int index) {
	return mInputAttributes[index];
}

Attribute* InstanceInformation::getOutputAttribute(int index) {
	return mOutputAttributes[index];
}

void InstanceInformation::addAttribute(Attribute* attribute, int index) {
	this->mInputAttributes[index] = attribute;
}

void InstanceInformation::addClass(Attribute* attribute, int index) {
	this->mOutputAttributes[index] = attribute;
}

double InstanceInformation::getAttributeInternalValue(int index,
		string strValue) {
	if (strValue == "?") {
		return NAN;
	}

	if (this->mNegativeOneAsNone && (atof(strValue.c_str()) == -1)) {
		return NAN;
	}

	if (mInputAttributes[index] == nullptr || mInputAttributes[index]->isNumeric()) {
		return (double) atof(strValue.c_str());
	} else {
		Attribute* att = mInputAttributes[index];
		for (int i = 0; i < att->numberValues(); i++) {
			if (att->getValue(i) == strValue) {
				return i;
			}
		}
		return NAN;
	}
}

double InstanceInformation::getOutputAttributeIndex(int index, const string& strValue) {
	if (mOutputAttributes[index] != nullptr) {
		Attribute* att = mOutputAttributes[index];
		for (int i = 0; i < att->numberValues(); i++) {
			if (att->getValue(i) == strValue) {
				return (double)i;
			}
		}
	}

	LOG_ERROR("%s is not a output attribute value!", strValue.c_str());
	return -1.0;
}

int InstanceInformation::getNumberInputAttributes() {
	return mInputAttributes.size();
}

int InstanceInformation::getNumberOutputAttributes() {
	return mOutputAttributes.size();
}

void InstanceInformation::setNumberInputAttributes(int count, vector<string>& vecValue) {
	if (this->mInputAttributes.size() == count) {
		return;
	}

	if (this->mInputAttributes.size() > count) {
		map<int, Attribute*> temp;
		for (auto iter = this->mInputAttributes.begin();
				iter != this->mInputAttributes.end(); iter++) {
			if (iter->first >= count) {
				delete iter->second;
			} else {
				temp[iter->first] = iter->second;
			}
		}
		this->mInputAttributes = temp;
	} else {
		if (!vecValue.empty()) {
			for (unsigned int i = this->mInputAttributes.size(); i < count; i++) {
				this->mInputAttributes[i] = new Attribute(vecValue);
			}
		}
		else {
			for (unsigned int i = this->mInputAttributes.size(); i < count; i++) {
				this->mInputAttributes[i] = new Attribute();
			}
		}
	}
}

InstanceInformation* InstanceInformation::clone() {
	InstanceInformation* ii = new InstanceInformation();
	for (auto iter = this->mInputAttributes.begin();
			iter != this->mInputAttributes.end(); iter++) {
		ii->addAttribute(iter->second->clone(), iter->first);
	}
	for (auto iter = this->mOutputAttributes.begin();
			iter != this->mOutputAttributes.end(); iter++) {
		ii->addClass(iter->second->clone(), iter->first);
	}
	return ii;
}
