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

#ifndef BAGGING_H
#define BAGGING_H


#include "../../Learner.h"
#include <vector>

class Bagging: public Learner {
public:
	Bagging();
	virtual ~Bagging();
	void train(const Instance&);
	virtual double* getPrediction(const Instance&);
	virtual string getEnsemblePrediction(const Instance&);

protected:
	vector<Learner*> learners;
	int numberClasses;
	virtual Learner* newLearner();

	virtual bool exportToJson(Json::Value& jv);
	virtual bool importFromJson(const Json::Value& jv);

private:
	int mEnsembleSize;
	string mLearnerName;
	string mLearnerParams;
	void doSetParams();
};

#endif //BAGGING_H
