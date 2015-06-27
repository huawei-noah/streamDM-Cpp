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

#ifndef _LOGISTICREGRESSION_H
#define _LOGISTICREGRESSION_H

#include "Perceptron.h"

class LogisticRegression: public Perceptron {
public:
	LogisticRegression();
	~LogisticRegression();

    virtual bool exportToJson(Json::Value& jv);
    virtual bool importFromJson(const Json::Value& jv);

private:
	double prediction(int actualClass, vector<double>& value, int);
	virtual double getWeightMultiplier(const Instance&, int);
	virtual double getInstanceMultiplier(int actualClass, vector<double>& value, int);
	double mLambda;
	double dloss(double);
	void initWeightAttributes();
	void doSetParams();
};

#endif //_LOGISTICREGRESSION_H
