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

#ifndef SIMPLENAIVEBAYESSTATISTICS_H_
#define SIMPLENAIVEBAYESSTATISTICS_H_

#include "NaiveBayesStatistics.h"
#include "observer/AttributeClassObserver.h"

#include <vector>
using namespace std;

class SimpleNaiveBayesStatistics: public NaiveBayesStatistics {
public:
	SimpleNaiveBayesStatistics();
	virtual ~SimpleNaiveBayesStatistics();

	virtual double probabilityOfClassAttrValue(int, int, double);
	virtual double probabilityOfClass(int);

    virtual void addObservation(int classLabel, 
        int attribute,
        bool isAttribNumeric,
        double value, 
        double weight);
    virtual void addObservation(int classLabel,
        double weight);

    bool exportToJson(Json::Value& jv);
    bool importFromJson(const Json::Value& jv);

private:
	vector<AttributeClassObserver*> m_attributeClassObservers;
	vector<int> m_classCounts;
	int m_sumWeight;
};

#endif /* SIMPLENAIVEBAYESSTATISTICS_H_ */
