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


#ifndef DENSEINSTANCE_H_
#define DENSEINSTANCE_H_

#include "Instance.h"
#include "../utils/json.h"

class DenseInstance: public Instance {
public:
	DenseInstance();
	virtual ~DenseInstance();

	//Values
	virtual double getInputAttributeValue(int) const;
	virtual double getOutputAttributeValue(int) const;
	virtual Json::Value toJson() const;

	virtual void addValues(const vector<double>& values);
	virtual void addLabels(const vector<double>& values);

	Instance* clone();

protected:
	vector<double> mInputData;
	vector<double> mOutputData;
};

#endif /* DENSEINSTANCE_H_ */
