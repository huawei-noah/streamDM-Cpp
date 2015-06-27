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

#include "DiscreteEstimator.h"

DiscreteEstimator::DiscreteEstimator() 
{
}

DiscreteEstimator::~DiscreteEstimator() 
{
}

void DiscreteEstimator::addObservation(int value, double weight) 
{
    m_attValDist.addToValue(value, weight);
}

double DiscreteEstimator::probabilityDensity(int value) 
{
    return (m_attValDist.getValue(value) + 1.0) / (m_attValDist.sumOfValues() + m_attValDist.numValues());
}

bool DiscreteEstimator::exportToJson(Json::Value& jv)
{
	Json::Value dvJson;
	m_attValDist.exportToJson(dvJson);
	jv["attVDist"] = dvJson;

	return true;
}

bool DiscreteEstimator::importFromJson(const Json::Value& jv)
{
	m_attValDist.importFromJson(jv["attVDist"]);

	return true;
}

