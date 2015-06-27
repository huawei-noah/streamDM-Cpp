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

#include "Learner.h"
#include "../Common.h"
#include <cmath>
#include <time.h>

Learner::Learner() {
	instancesSeen = 0;
	init = false;
	this->evaluator = nullptr;
}
Learner::~Learner() {
}

void Learner::process(const Instance &inst) {
	//test

	if (instancesSeen > 0) {
		evaluator->addResult(inst, this->getPrediction(inst));
	}
	instancesSeen++;
	//train
	train(inst);
}

void Learner::trainBagging(const Instance &inst) {
	int weight = Utils::poisson(1.0);
	while (weight > 0) {
		weight--;
		train(inst);
	}
}

int Learner::getInstanceSeen() {
	return instancesSeen;
}

int Learner::predict(const Instance& instance) {
		double numberClasses = instance.getNumberClasses();
	    double* classPredictions = getPrediction(instance);
		int result = 0;
		double max = classPredictions[0];
		//Find class label with higher probability
		for (int i = 1; i < numberClasses; i++) {
			if (max < classPredictions[i]) {
				max = classPredictions[i];
				result = i;
			}
		}
		return result;
}

void Learner::setEvaluator(Evaluator* ev) {
	this->evaluator = ev;
}

Evaluator* Learner::getEvaluator(){
	return evaluator;
}

string Learner::getEnsemblePrediction(const Instance&) {
	return "";
}

