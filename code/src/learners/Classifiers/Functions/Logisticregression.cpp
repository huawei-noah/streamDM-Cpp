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

#include "Logisticregression.h"
#include "../../../Common.h"
#include <cmath>

REGISTER_CLASS(LogisticRegression);
REGISTER_COMMAND_LINE_PARAMETER(
		LogisticRegression,
		"{\"type\":\"Learner\","
		"\"name\":\"LogisticRegression\","
		"\"parameter\":{"
		"\"-r\":\"LearningRatio\","
		"\"-a\":\"Lambda\""
		"}}"
		"");

LogisticRegression::LogisticRegression() : Perceptron() {
	mLambda = 0.0001;
}

LogisticRegression::~LogisticRegression() {
}

void LogisticRegression::doSetParams() {
    mLearningRatio = getParam("LearningRatio", 0.01);
	mLambda = getParam("Lambda", 0.0001);
}

double LogisticRegression::prediction(int actualClass, vector<double>& value, int label) {
	double z = dotProd(actualClass,value, label);
	z += getWeightAttributes(label, numberInputAttributes);
	double ret;
	if (z <= 0){
		ret = 1.0 - (1.0/ (1.0+ exp(z)));
	}else {
		ret = 1.0/ (1.0+ exp(-z));
	}
	return ret;
}

double LogisticRegression::getInstanceMultiplier(int actualClass, vector<double>& value, int classLabel){
		double y = (int) actualClass != classLabel ? -1.0 : 1.0;
		double wx = dotProd(actualClass,value, classLabel);
	    double z = y * (wx + getWeightAttributes(classLabel, numberInputAttributes));
	    double factor = y * mLearningRatio * dloss(z);
	    return factor;
}

double LogisticRegression::getWeightMultiplier(const Instance& instance, int classLabel){
	    double multiplier = 1.0 - (mLearningRatio * mLambda)/((double)instancesSeen+1);
	    return multiplier;
}
double LogisticRegression::dloss(double z){
	double ret;
	if (z < 0){
		ret = 1.0/ (1.0+ exp(z));
	}else {
		double t = exp(-z);
		ret = t/ (t+1.0);	
	}
	return ret;
}

void LogisticRegression::initWeightAttributes() {
	for (int i = 0; i < numberClasses; i++) {
		weightAttributes[i].resize(numberInputAttributes+1);
		for (int j = 0; j < numberInputAttributes+1; j++) {
			setWeightAttributes(0, i, j);
		}
	}
}

bool LogisticRegression::exportToJson(Json::Value& jv) {
	Perceptron::exportToJson(jv);
	jv["lambda"] = mLambda;
	return true;
}

bool LogisticRegression::importFromJson(const Json::Value& jv) {
	Perceptron::importFromJson(jv);
	mLambda = jv["lambda"].asDouble();
	return true;
}
