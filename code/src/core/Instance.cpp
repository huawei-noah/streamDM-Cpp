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

#include "Instance.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fstream> 
#include <sstream>  
#include <iostream>

Instance::Instance() {
	weight = 1;
	this->instanceInformation = nullptr;
	this->instanceInformationSaved = false;
}

Instance::~Instance() {
	if (this->instanceInformationSaved
			&& this->instanceInformation != nullptr) {
		delete this->instanceInformation;
	}
}

int Instance::getNumberClasses() const {
	return instanceInformation->getNumberClasses();
}

Attribute* Instance::getInputAttribute(int index) const {
	return instanceInformation->getInputAttribute(index);
}

Attribute* Instance::getOutputAttribute(int index) {
	return instanceInformation->getOutputAttribute(index);
}

InstanceInformation* Instance::getInstanceInformation() const {
	return instanceInformation;
}

void Instance::setInstanceInformation(InstanceInformation* ii) {
	this->instanceInformationSaved = false;
	this->instanceInformation = ii;
}

void Instance::saveInstanceInformation(InstanceInformation* ii) {
	this->instanceInformationSaved = true;
	this->instanceInformation = ii->clone();
}

double Instance::getLabel() const {
	return this->getOutputAttributeValue(0);
}

double Instance::getWeight() const {
	return weight;
}

void Instance::setWeight(double v) {
	weight = v;
}

int Instance::getNumberInputAttributes() const {
	return instanceInformation->getNumberInputAttributes();
}

int Instance::getNumberOutputAttributes() const {
	return instanceInformation->getNumberOutputAttributes();
}

void Instance::addValues(const vector<double>& values) {

}

void Instance::addLabels(const vector<double>& values) {

}

Instance* cloneInstance(const Instance* s) {
	Instance* temp = (Instance*) s;
	return temp->clone();
}

list<list<double> >* Instance::getValues() {
	return nullptr;
}
