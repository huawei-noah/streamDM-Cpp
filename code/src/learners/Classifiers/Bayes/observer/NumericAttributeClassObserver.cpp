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

#include "NumericAttributeClassObserver.h"

NumericAttributeClassObserver::NumericAttributeClassObserver()
{
    m_attValDistPerClass.clear();
}

NumericAttributeClassObserver::~NumericAttributeClassObserver()
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

void NumericAttributeClassObserver::observeClassAttributeValue(int classVal, double attVal, double weight)
{
    if (std::isnan(attVal))
    {
        return;
    }

    GaussianEstimator* valDist = NULL;

    if (m_attValDistPerClass.size() > classVal)
    {
        valDist = m_attValDistPerClass[classVal];
    }
    else
    {
        m_attValDistPerClass.resize(classVal+1, NULL);
    }
    
    if (valDist == NULL) 
    {
        valDist = new GaussianEstimator();
        m_attValDistPerClass[classVal] = valDist;

        m_minValueObservedPerClass.setValue(classVal, attVal);
        m_maxValueObservedPerClass.setValue(classVal, attVal);
    } 
    else 
    {
        if (attVal < m_minValueObservedPerClass.getValue(classVal)) 
        {
            m_minValueObservedPerClass.setValue(classVal, attVal);
        }

        if (attVal > m_maxValueObservedPerClass.getValue(classVal)) 
        {
            m_maxValueObservedPerClass.setValue(classVal, attVal);
        }
    }

    valDist->addObservation(attVal, weight);
}

double NumericAttributeClassObserver::probabilityOfClassAttrValue(int classVal, double attVal)
{
    if (classVal >= m_attValDistPerClass.size() || NULL == m_attValDistPerClass[classVal])
    {
        return 0.0;
    }

    return m_attValDistPerClass[classVal]->probabilityDensity(attVal);
}

bool NumericAttributeClassObserver::exportToJson(Json::Value& jv)
{
	jv["name"] = "Numeric";

	Json::Value minJson;
	m_minValueObservedPerClass.exportToJson(minJson);
	jv["minVOPClass"] = minJson;

	Json::Value maxJson;
	m_maxValueObservedPerClass.exportToJson(maxJson);
	jv["maxVOPClass"] = maxJson;

	for (GaussianEstimator* pGE : m_attValDistPerClass)
	{
		Json::Value geJson;

		if (nullptr == pGE) {
			//geJson.append("null");
		}
		else {
			pGE->exportToJson(geJson);
		}

		jv["attVDPClass"].append(geJson);
	}

	return true;
}

bool NumericAttributeClassObserver::importFromJson(const Json::Value& jv)
{
	m_minValueObservedPerClass.importFromJson(jv["minVOPClass"]);
	m_maxValueObservedPerClass.importFromJson(jv["maxVOPClass"]);

	int iSize = jv["attVDPClass"].size();
	m_attValDistPerClass.resize(iSize);

	for (int i = 0; i < iSize; ++i)
	{
		const Json::Value& geJson = jv["attVDPClass"][i];
		if (geJson.isNull()) {
			m_attValDistPerClass[i] = nullptr;
		}
		else {
			m_attValDistPerClass[i] = new GaussianEstimator();
			m_attValDistPerClass[i]->importFromJson(geJson);
		}
	}

	return true;
}


