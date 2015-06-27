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

#ifndef BASICCLASSIFICATIONEVALUATOR_H_
#define BASICCLASSIFICATIONEVALUATOR_H_

#include "Evaluator.h"

class BasicClassificationEvaluator: public Evaluator {
public:
	// BasicClassificationEvaluator(int);
	BasicClassificationEvaluator();
	virtual ~BasicClassificationEvaluator();
	virtual void addResult(const Instance&, double[]);
	void doSetParams();

protected:
	double instancesSeen;
	double instancesCorrectlyClassified;
	int getLabel(double[], int);
	int frequency;
	vector<vector<double> > matrix;
	vector<double> columnSum;
	vector<double> rowSum;
	double getKappaStatistic();
	int numClasses;

};

#endif /* BASICCLASSIFICATIONEVALUATOR_H_ */
