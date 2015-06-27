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

#ifndef PERCEPTRON_H
#define PERCEPTRON_H

#define LEARNINGRATIO 1

#include "../../Learner.h"

class Perceptron: public Learner {
public:
	Perceptron();
	virtual ~Perceptron();
	void train(const Instance&);
	double* getPrediction(const Instance&);
	double getWeightAttributes(int, int);

    virtual bool exportToJson(Json::Value& jv);
    virtual bool importFromJson(const Json::Value& jv);

protected:
	vector<vector<double> > weightAttributes;
	double* classPrediction;
	void setWeightAttributes(double, int, int);
	void addWeightAttributes(double, int, int);
	void multiplyWeightAttributes(double, int, int);
	int numberClasses;
	int numberInputAttributes;
	virtual double prediction(int actualClass, vector<double>& value, int);
	double dotProd(int actualClass, vector<double>& value, int);
	virtual double getWeightMultiplier(const Instance&, int);
	virtual double getInstanceMultiplier(int actualClass, vector<double>& value, int);
	double mLearningRatio;
	virtual void initWeightAttributes();
private:
	void doSetParams();
};

#endif //PERCEPTRON_H
