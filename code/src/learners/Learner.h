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

#ifndef LEARNER_H
#define LEARNER_H

#include "../core/Instance.h"
#include "../evaluation/Evaluator.h"
#include "../utils/Configurable.h"
#include "../utils/LearnerModel.h"

using namespace std;
class Learner : public Configurable, public LearnerModel {
public:
	Learner();
	virtual ~Learner();
	virtual void train(const Instance&) = 0;
	virtual double* getPrediction(const Instance&) = 0;
	int predict(const Instance&);
	void trainBagging(const Instance&);
	void process(const Instance&);
	int getInstanceSeen();
	void setEvaluator(Evaluator*);
	virtual void initPara(int argc, char* argv[]) {};
	Evaluator* getEvaluator();
	virtual string getEnsemblePrediction(const Instance&);

protected:
	int instancesSeen;
	bool init;
	Evaluator* evaluator;

};

#endif //LEARNER_H