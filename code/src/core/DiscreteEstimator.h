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


#ifndef DISCRETEESTIMATOR_H_
#define DISCRETEESTIMATOR_H_

#include <vector>
#include "DoubleVector.h"

class DiscreteEstimator {
public:
	DiscreteEstimator();
	virtual ~DiscreteEstimator();

	void addObservation(int value, double weight);
	double probabilityDensity(int value);

    bool exportToJson(Json::Value& jv);
    bool importFromJson(const Json::Value& jv);

protected:
    DoubleVector m_attValDist;
};

#endif /* DISCRETEESTIMATOR_H_ */
