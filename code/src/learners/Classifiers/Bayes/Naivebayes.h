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

#ifndef NAIVEBAYES_H
#define NAIVEBAYES_H

#include "../../Learner.h"
#include "NaiveBayesStatistics.h"

class NaiveBayes: public Learner {
public:
	NaiveBayes();
	~NaiveBayes();
	void train(const Instance&);
	double* getPrediction(const Instance&);
	double probability(const Instance&, int);

    virtual bool exportToJson(Json::Value& jv);
    virtual bool importFromJson(const Json::Value& jv);

private:
	NaiveBayesStatistics* nbStatistics;
	void doSetParams();
};

#endif //NAIVEBAYES_H
