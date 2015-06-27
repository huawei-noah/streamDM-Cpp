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

#include "Perceptron.h"
#include "../../../Common.h"
#include <cmath>

REGISTER_CLASS(Perceptron);
REGISTER_COMMAND_LINE_PARAMETER(Perceptron, "{\"type\":\"Learner\","
		"\"name\":\"Perceptron\","
		"\"parameter\":{"
		"\"-r\":\"LearningRatio\""
		"}}"
		"");

Perceptron::Perceptron():
	classPrediction(nullptr),
	numberInputAttributes(0),
	numberClasses(0)
{
	init = false;
	instancesSeen = 0;
	mLearningRatio = 1.0;
}

Perceptron::~Perceptron() {
	if (init) {
		delete[] classPrediction;
	}
}

void Perceptron::doSetParams() {
	mLearningRatio = getParam("LearningRatio", 1.0);
}

void Perceptron::initWeightAttributes() {
	for (int i = 0; i < numberClasses; i++) {
		weightAttributes[i].resize(numberInputAttributes+1);
		for (int j = 0; j < numberInputAttributes+1; j++) {
			setWeightAttributes(0.2 * (rand() % 1000) / 1000 - 0.1, i, j);
		}
	}
}

void Perceptron::train(const Instance& instance) {
	if (!init) {
		init = true;
		numberClasses = instance.getNumberClasses();
		numberInputAttributes = instance.getNumberInputAttributes();
		weightAttributes.resize(numberClasses);
		classPrediction = new double[numberClasses];
		initWeightAttributes();
		instancesSeen = 0;
		mLearningRatio = 1.0;
	}

	vector<double> value(numberInputAttributes);
	for (int i = 0; i < numberInputAttributes; i++) {
		value[i] = instance.getInputAttributeValue(i);
	}
	int actualClass = (int) instance.getLabel();

	for (int i = 0; i < numberClasses; i++) {
		//Train one perceptron for each class
		double weightMultiplier = getWeightMultiplier(instance, i);
		double instanceMultiplier = getInstanceMultiplier(actualClass, value, i);
		for (int j = 0; j < numberInputAttributes; j++) {
			multiplyWeightAttributes(weightMultiplier, i, j);
			addWeightAttributes(
					instanceMultiplier * value[j], i,
					j);
		}
		multiplyWeightAttributes(weightMultiplier, i, numberInputAttributes);
		addWeightAttributes(instanceMultiplier, i, numberInputAttributes);
	}
}

double Perceptron::getWeightMultiplier(const Instance& instance, int i) {
	return 1.0;
}

double Perceptron::getInstanceMultiplier(int actualClass, vector<double>& value, int i) {
	double pred = prediction(actualClass, value, i);
	double actual = (i == actualClass) ? 1.0 : 0.0;
	double delta = (actual - pred) * pred * (1 - pred);
	return mLearningRatio * delta;
}
double Perceptron::prediction(int actualClass, vector<double>& value, int label) {
	double sum = dotProd(actualClass, value, label);
	sum += getWeightAttributes(label, numberInputAttributes);
	//return sum;
	return 1.0 / (1.0 + exp(-sum));
}

double Perceptron::dotProd(int actualClass, vector<double>& value, int label) {
	double ret = 0;
	for (int i = 0; i < numberInputAttributes; i++) {
		ret += getWeightAttributes(label, i)
				* value[i];
	}
	return ret;
}

double* Perceptron::getPrediction(const Instance& instance) {
	if (!init) {
		init = true;
		numberClasses = instance.getNumberClasses();
		numberInputAttributes = instance.getNumberInputAttributes();
		weightAttributes.resize(numberClasses);
		classPrediction = new double[numberClasses];
		initWeightAttributes();
		instancesSeen = 0;
		mLearningRatio = 1.0;

		for (int i = 0; i < numberClasses; i++) {
			classPrediction[i] = 0.0;
		}
		return classPrediction;
	}

	vector<double> value(numberInputAttributes);
	for (int i = 0; i < numberInputAttributes; i++) {
		value[i] = instance.getInputAttributeValue(i);
	}
	int actualClass = (int) instance.getLabel();
	for (int i = 0; i < numberClasses; i++) {
		classPrediction[i] = prediction(actualClass, value, i);
	}
	return classPrediction;
}
double Perceptron::getWeightAttributes(int i, int j) {
	return weightAttributes[i][j];
}
void Perceptron::setWeightAttributes(double value, int i, int j) {
	weightAttributes[i][j] = value;
}
void Perceptron::addWeightAttributes(double value, int i, int j) {
	weightAttributes[i][j] += value;
}
void Perceptron::multiplyWeightAttributes(double value, int i, int j) {
	weightAttributes[i][j] *= value;
}

bool Perceptron::exportToJson(Json::Value& jv) {
	jv["nClasses"] = numberClasses;
	jv["nInputAttributes"] = numberInputAttributes;
	jv["learningRatio"] = mLearningRatio;

	for (unsigned int i = 0; i < weightAttributes.size(); ++i) {
		Json::Value attribJson;
		for (unsigned int j = 0; j < weightAttributes[i].size(); ++j) {
			attribJson[i].append(weightAttributes[i][j]);
		}
		jv["weightAttributes"].append(attribJson);
	}

	return true;
}

bool Perceptron::importFromJson(const Json::Value& jv) {
	numberClasses = jv["nClasses"].asInt();
	numberInputAttributes = jv["nInputAttributes"].asInt();
	mLearningRatio = jv["learningRatio"].asDouble();

	unsigned int iSize = jv["weightAttributes"].size();
	weightAttributes.resize(iSize);

	for (unsigned int i = 0; i < iSize; ++i) {
		const Json::Value& attribJson = jv["weightAttributes"][i];
		weightAttributes[i].resize(attribJson.size());
		for (unsigned int j = 0; j < attribJson.size(); ++j) {
			weightAttributes[i][j]= attribJson[j].asDouble();
		}
	}

	return true;
}

