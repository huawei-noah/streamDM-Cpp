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

#include "InstanceConditionalTest.h"
#include <cmath>

namespace HT {

InstanceConditionalTest::InstanceConditionalTest() {
}

InstanceConditionalTest::~InstanceConditionalTest() {
}

bool InstanceConditionalTest::resultKnownForInstance(const Instance* inst) {
	return branchForInstance(inst) >= 0;
}

int InstanceConditionalBinaryTest::maxBranches() {
	return 2;
}

NumericAttributeBinaryTest::NumericAttributeBinaryTest(int attIndex,
		double attValue, bool equalsPassesTest) {
	this->attIndex = attIndex;
	this->attValue = attValue;
	this->equalsPassesTest = equalsPassesTest;
	this->mClassTypes = {T_NumericAttributeBinaryTest, T_InstanceConditionalBinaryTest, T_InstanceConditionalTest};
}

NumericAttributeBinaryTest::NumericAttributeBinaryTest(const Json::Value& jv) {
	this->attIndex = jv["attIndex"].asInt();
	this->attValue = jv["attValue"].asDouble();
	this->equalsPassesTest = jv["equalsPassesTest"].asBool();
	this->mClassTypes = {T_NumericAttributeBinaryTest, T_InstanceConditionalBinaryTest, T_InstanceConditionalTest};
}

void NumericAttributeBinaryTest::toJson(Json::Value& jv) {
	jv["type"] = "NumericAttributeBinaryTest";
	jv["attIndex"] = this->attIndex;
	jv["attValue"] = this->attValue;
	jv["equalsPassesTest"] = this->equalsPassesTest;
	jv["maxBranches"] = this->maxBranches();
}

int NumericAttributeBinaryTest::getAttIndex() {
	return this->attIndex;
}

double NumericAttributeBinaryTest::getAttValue() {
	return this->attValue;
}


int NumericAttributeBinaryTest::branchForInstance(const Instance* inst) {
//	int instAttIndex =
//			this->attIndex < inst->getLabel() ?
//					this->attIndex : this->attIndex + 1;
	int instAttIndex = this->attIndex;
	if (instAttIndex >= inst->getNumberInputAttributes()) {
		return -1;
	}
	if (std::isnan(inst->getInputAttributeValue(instAttIndex))) {
		return -1;
	}
	double v = inst->getInputAttributeValue(instAttIndex);
	if (v == this->attValue) {
		return this->equalsPassesTest ? 0 : 1;
	}
	return v < this->attValue ? 0 : 1;
}

vector<int>* NumericAttributeBinaryTest::getAttsTestDependsOn() {
	return new vector<int>{this->attIndex};
}

double NumericAttributeBinaryTest::getSplitValue() {
	return this->attValue;
}

InstanceConditionalTest* NumericAttributeBinaryTest::clone() {
	return new NumericAttributeBinaryTest(this->attIndex, this->attValue,
			this->equalsPassesTest);
}


// NominalAttributeMultiwayTest

NominalAttributeMultiwayTest::NominalAttributeMultiwayTest(int attIndex) {
	this->attIndex = attIndex;
	this->mClassTypes = {T_NominalAttributeMultiwayTest, T_InstanceConditionalTest};
}

NominalAttributeMultiwayTest::NominalAttributeMultiwayTest(
		const Json::Value& jv) {
	this->attIndex = jv["attIndex"].asInt();
	this->mClassTypes = {T_NominalAttributeMultiwayTest, T_InstanceConditionalTest};
}

void NominalAttributeMultiwayTest::toJson(Json::Value& jv) {
	jv["type"] = "NominalAttributeMultiwayTest";
	jv["attIndex"] = this->attIndex;
	jv["maxBranches"] = this->maxBranches();
}

int NominalAttributeMultiwayTest::getAttIndex() {
	return this->attIndex;
}

double NominalAttributeMultiwayTest::getAttValue() {
	return NAN;
}

int NominalAttributeMultiwayTest::branchForInstance(const Instance* inst) {
//	int instAttIndex =
//			this->attIndex < inst->getLabel() ?
//					this->attIndex : this->attIndex + 1;
	int instAttIndex = this->attIndex;
	if (instAttIndex >= inst->getNumberInputAttributes()) {
		return -1;
	}
	return std::isnan(inst->getInputAttributeValue(instAttIndex)) ?
			-1 : (int) inst->getInputAttributeValue(instAttIndex);
}

int NominalAttributeMultiwayTest::maxBranches() {
	return -1;
}

vector<int>* NominalAttributeMultiwayTest::getAttsTestDependsOn() {
	return new vector<int>(1, this->attIndex);
}

InstanceConditionalTest* NominalAttributeMultiwayTest::clone() {
	return new NominalAttributeMultiwayTest(this->attIndex);
}


// NominalAttributeBinaryTest

NominalAttributeBinaryTest::NominalAttributeBinaryTest(int attIndex,
		int attValue) {
	this->attIndex = attIndex;
	this->attValue = attValue;
	this->mClassTypes = {T_NominalAttributeBinaryTest, T_InstanceConditionalBinaryTest};
}

NominalAttributeBinaryTest::NominalAttributeBinaryTest(const Json::Value& jv) {
	this->attIndex = jv["attIndex"].asInt();
	this->attValue = jv["attValue"].asDouble();
	this->mClassTypes = {T_NominalAttributeBinaryTest, T_InstanceConditionalBinaryTest};
}

void NominalAttributeBinaryTest::toJson(Json::Value& jv) {
	jv["type"] = "NominalAttributeBinaryTest";
	jv["attIndex"] = this->attIndex;
	jv["attValue"] = this->attValue;
	jv["maxBranches"] = this->maxBranches();
}

int NominalAttributeBinaryTest::getAttIndex() {
	return this->attIndex;
}

double NominalAttributeBinaryTest::getAttValue() {
	return this->attValue;
}

int NominalAttributeBinaryTest::branchForInstance(const Instance* inst) {
//	int instAttIndex =
//			this->attIndex < inst->getLabel() ?
//					this->attIndex : this->attIndex + 1;
	int instAttIndex = this->attIndex;
	if (instAttIndex >= inst->getNumberInputAttributes()) {
		return -1;
	}
	return std::isnan(inst->getInputAttributeValue(instAttIndex)) ?
			-1 :
			((int) inst->getInputAttributeValue(instAttIndex)
					== this->attValue ? 0 : 1);
}

vector<int>* NominalAttributeBinaryTest::getAttsTestDependsOn() {
	return new vector<int>{this->attIndex};
}

InstanceConditionalTest* NominalAttributeBinaryTest::clone() {
	return new NominalAttributeBinaryTest(this->attIndex, this->attValue);
}



} /* namespace HT */
