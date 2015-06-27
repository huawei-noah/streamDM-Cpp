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

#ifndef NAIVEBAYESSTATISTICS_H_
#define NAIVEBAYESSTATISTICS_H_

#include "../../../utils/json.h"

class NaiveBayesStatistics {
public:
	NaiveBayesStatistics();
	virtual ~NaiveBayesStatistics();

	virtual double probabilityOfClassAttrValue(int, int, double) = 0;
	virtual double probabilityOfClass(int) = 0;
    virtual void addObservation(int classLabel, 
        int attribute,
        bool isAttribNumeric,
        double value, 
        double weight) = 0;
    virtual void addObservation(int classLabel,
    	double weight) = 0;

    virtual bool exportToJson(Json::Value& jv) = 0;
    virtual bool importFromJson(const Json::Value& jv) = 0;
};

#endif /* NAIVEBAYESSTATISTICS_H_ */
