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

#include "Attribute.h"

Attribute::Attribute() {
	mIsNumeric = true;
}

Attribute::Attribute(const vector<string>& values) {
	mIsNumeric = false;
	mValues = values;
}

Attribute::~Attribute() {
}

Json::Value Attribute::toJson() {
	Json::Value jv;
	jv["IsNumeric"] = mIsNumeric;
	if (!mIsNumeric) {
		int size = (int)mValues.size();
		for (int i=0; i<size; i++) {
			jv["values"][i] = mValues[i];
		}
	}
	return jv;
}

bool Attribute::isNumeric() {
	return mIsNumeric;
}

bool Attribute::isNominal() {
	return !mIsNumeric;
}

int Attribute::numberValues() {
	return this->mValues.size();
}

std::string Attribute::getValue(int index) {
	return this->mValues[index];
}

Attribute* Attribute::clone() {
	if (this->mIsNumeric) {
		return new Attribute();
	}else{
		return new Attribute(this->mValues);
	}
}


