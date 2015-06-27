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

#ifndef INSTANCE_H
#define INSTANCE_H

#include <string>
#include <list>
#include "InstanceInformation.h"
#include "../utils/json.h"

using namespace std;

class Instance {
public:
	Instance();
	virtual ~Instance();

	//Values
	virtual double getInputAttributeValue(int) const = 0;
    virtual int getInputAttributeID(int) const { return 0;}
	virtual double getOutputAttributeValue(int) const = 0;
	double getLabel() const;
	virtual Json::Value toJson() const = 0;
	
	//Weight
	double getWeight() const;
	void setWeight(double v);
    virtual float getSquaredNorm() const { return 0.0; }

	//InstanceInformation
	virtual int getNumberInputAttributes() const;
	int getNumberOutputAttributes() const;
	virtual int getNumberClasses() const;
	Attribute* getInputAttribute(int) const;
	Attribute* getOutputAttribute(int);
	InstanceInformation* getInstanceInformation() const;
	void setInstanceInformation(InstanceInformation*);
	void saveInstanceInformation(InstanceInformation*);

//	virtual void setNumValues(int num) = 0;
	virtual void addValues(const vector<double>& values);

	// only used in FPInstance
	virtual list<list<double>>* getValues();

	virtual void addLabels(const vector<double>& values);

	virtual Instance* clone() = 0;

protected:
	InstanceInformation* instanceInformation;
	bool instanceInformationSaved;
	double weight;
};

Instance* cloneInstance(const Instance* s);

#endif
