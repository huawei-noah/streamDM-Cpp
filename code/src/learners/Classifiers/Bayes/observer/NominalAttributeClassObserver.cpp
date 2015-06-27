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

#include "NominalAttributeClassObserver.h"

//////////////////////////////////////////////////////////////////////////

NominalAttributeClassObserver::NominalAttributeClassObserver()
{
    m_attValDistPerClass.clear();
}

NominalAttributeClassObserver::~NominalAttributeClassObserver()
{
    for (unsigned int i = 0; i < m_attValDistPerClass.size(); ++i)
    {
        if (NULL != m_attValDistPerClass[i])
        {
            delete m_attValDistPerClass[i];
            m_attValDistPerClass[i] = NULL;
        }
    }

    m_attValDistPerClass.clear();
}

//////////////////////////////////////////////////////////////////////////

void NominalAttributeClassObserver::observeClassAttributeValue(int classVal, double attVal, double weight)
{
    if (std::isnan(attVal))
    {
        return;
    }

    DiscreteEstimator* pDEstimator = NULL;

    if (m_attValDistPerClass.size() > classVal)
    {
        pDEstimator = m_attValDistPerClass[classVal];    
    }
    else
    {
        m_attValDistPerClass.resize(classVal + 1, NULL);   
    }

    if (NULL == pDEstimator)
    {
        pDEstimator = new DiscreteEstimator();
        m_attValDistPerClass[classVal] = pDEstimator;
    }
    
    pDEstimator->addObservation((int)attVal, weight);
}

double NominalAttributeClassObserver::probabilityOfClassAttrValue(int classVal, double attVal)
{
    if (classVal >= m_attValDistPerClass.size() || NULL == m_attValDistPerClass[classVal])
    {
        return 0.0;
    }

    return m_attValDistPerClass[classVal]->probabilityDensity((int)attVal);
}

bool NominalAttributeClassObserver::exportToJson(Json::Value& jv)
{
	jv["name"] = "Nominal";

	for (DiscreteEstimator* pDE : m_attValDistPerClass)
	{
		Json::Value deJson;

		if (nullptr == pDE) {
			deJson.append("null");
		}
		else {
			pDE->exportToJson(deJson);
		}

		jv["attVDPClass"].append(deJson);
	}

	return true;
}

bool NominalAttributeClassObserver::importFromJson(const Json::Value& jv)
{
	int iSize = jv["attVDPClass"].size();
	m_attValDistPerClass.resize(iSize);

	for (int i = 0; i < iSize; ++i)
	{
		const Json::Value& deJson = jv["attVDPClass"][i];
		if ("null" == deJson.toStyledString()) {
			m_attValDistPerClass[i] = nullptr;
		}
		else {
			m_attValDistPerClass[i] = new DiscreteEstimator();
			m_attValDistPerClass[i]->importFromJson(deJson);
		}
	}

	return true;
}

