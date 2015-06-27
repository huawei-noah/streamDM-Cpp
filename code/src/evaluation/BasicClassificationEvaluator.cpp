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

#include "BasicClassificationEvaluator.h"
#include "../Common.h"

const int MeasureInstancesSeen = 0;
const int MeasureAccuracy = 1;
const int MeasureKappaStatistic = 2;
const int MeasurePrecision = 3;
const int MeasureRecall = 4;
const int MeasureF1 = 5;

REGISTER_CLASS(BasicClassificationEvaluator)
REGISTER_COMMAND_LINE_PARAMETER(
		BasicClassificationEvaluator,
		"{\"type\":\"Evaluator\","
		"\"name\":\"BasicClassificationEvaluator\","
		"\"parameter\":{"
		"\"-f\":\"Frequency\""
		"}}"
		"");


BasicClassificationEvaluator::BasicClassificationEvaluator() {
	instancesCorrectlyClassified = 0;
	instancesSeen = 0;
	measures = new Measures();
	measures->addName("InstancesSeen", MeasureInstancesSeen);
	measures->addName("Accuracy", MeasureAccuracy);
	measures->addName("Kappa Statistic",MeasureKappaStatistic);
	measures->addName("Class 0: Precision",MeasurePrecision);
	measures->addName("Class 0: Recall",MeasureRecall);
	measures->addName("Class 0: F1",MeasureF1);


	frequency = 10000;	// default value
}

BasicClassificationEvaluator::~BasicClassificationEvaluator() {
	delete measures;
}

void BasicClassificationEvaluator::doSetParams() {
	frequency = getParam("Frequency", 10000);
}

void BasicClassificationEvaluator::addResult(const Instance& instance,
		double prediction[]) {
	double weight = instance.getWeight();
	if (this->instancesSeen == 0){
		this->numClasses = instance.getNumberClasses();
		rowSum.resize(this->numClasses);
		columnSum.resize(this->numClasses);
		matrix.resize(this->numClasses);
		for(int i=0; i < this->numClasses; i++){
			matrix[i].resize(this->numClasses);
		}
	}
	if (weight > 0) {
		    int trueClass =  instance.getLabel();
		    int predictedClass =  getLabel(prediction, instance.getNumberClasses());
			if ( trueClass == predictedClass) {
				instancesCorrectlyClassified += weight;

			}
			this->rowSum[predictedClass] += weight;
			this->columnSum[trueClass] += weight;
			this->matrix[predictedClass][trueClass] += weight;

	}
	instancesSeen += weight;
	if (((int) instancesSeen) % frequency == 0) {
		//Report Statistics
		vector<double> measure;
		measure.resize(6);
		measure[MeasureInstancesSeen] = instancesSeen;
		measure[MeasureAccuracy] = instancesCorrectlyClassified/instancesSeen;
		measure[MeasureKappaStatistic] = getKappaStatistic();
		measure[MeasurePrecision] = this->matrix[0][0]/ this->rowSum[0];
		measure[MeasureRecall] = this->matrix[0][0] / this->columnSum[0];
		measure[MeasureF1] = (2.0 * this->matrix[0][0]) /  ( 2.0 * this->matrix[0][0] + this->columnSum[0] + this->rowSum[0]);
		measures->addMeasures(measure);
	}
}


double BasicClassificationEvaluator::getKappaStatistic() {
	double ret = 0.0;
	if (this->instancesSeen > 0){
		double p0 = (double)instancesCorrectlyClassified/(double) instancesSeen;
		double pc = 0.0;
		for(int i=0; i < this->numClasses; i++){
			pc += (this->rowSum[i]/this->instancesSeen) * (this->columnSum[i]/this->instancesSeen);
		}
		ret = (p0-pc)/(1-pc);
	}
	return ret;
}

int BasicClassificationEvaluator::getLabel(double classPredictions[], int numberClasses) {
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
