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

#include "SimpleNaiveBayesStatistics.h"
#include "observer/NumericAttributeClassObserver.h"
#include "observer/NominalAttributeClassObserver.h"
#include <stdio.h>

SimpleNaiveBayesStatistics::SimpleNaiveBayesStatistics():m_sumWeight(0)
{
    m_attributeClassObservers.clear();
    m_classCounts.clear();
}

SimpleNaiveBayesStatistics::~SimpleNaiveBayesStatistics() 
{
    for (unsigned int i = 0; i < m_attributeClassObservers.size(); ++i)
    {
        if (NULL != m_attributeClassObservers[i])
        {
            delete m_attributeClassObservers[i];
            m_attributeClassObservers[i] = NULL;
        }
    }

    m_attributeClassObservers.clear();
    m_classCounts.clear();
}

double SimpleNaiveBayesStatistics::probabilityOfClassAttrValue(int classLabel,
		int attribute, double value) 
{
    //if (attribute >= m_attributeClassObservers.size())
	if (NULL == m_attributeClassObservers[attribute])
	{
        //LOG_DEBUG( "SimpleNBS::probabilityOfClassAttrValue>>attribute(%d), ob size(%d)",
        //    classLabel, m_attributeClassObservers.size());

        return 1.0;
    }

	return m_attributeClassObservers[attribute]->probabilityOfClassAttrValue(classLabel, value);
}

double SimpleNaiveBayesStatistics::probabilityOfClass(int classLabel) 
{
    if (classLabel >= m_classCounts.size())
    {
        //LOG_ERROR( "SimpleNBS::probabilityOfClass>>classLabel(%d), classCounts size(%d)", classLabel, m_classCounts.size());
        return 0.0;
    }
    //LOG_DEBUG( "SimpleNBS::probabilityOfClass>>classLabel(%d), classCounts size(%d)", classLabel, m_classCounts.size());

	return (double) m_classCounts[classLabel] / (double) m_sumWeight;
}

void SimpleNaiveBayesStatistics::addObservation(int classLabel, 
                                                double weight)
{
	//Add classsCounts
    if (classLabel >= m_classCounts.size())
    {
        m_classCounts.resize(classLabel+1, 0);
    }

	m_classCounts[classLabel] += weight;
	m_sumWeight += weight;

}

void SimpleNaiveBayesStatistics::addObservation(int classLabel,
                                                int attribute,
                                                bool isAttributeNumeric,
                                                double value,
                                                double weight)
{
    //Observers are added in here incrementally
    if ((unsigned int)attribute >= m_attributeClassObservers.size())
    {
        m_attributeClassObservers.resize(attribute+1, NULL);
    }

    if (NULL == m_attributeClassObservers[attribute])
    {
        if (isAttributeNumeric)
        {
            m_attributeClassObservers[attribute] = new NumericAttributeClassObserver();
        }
        else
        {
            m_attributeClassObservers[attribute] = new NominalAttributeClassObserver();
        }
    }

	m_attributeClassObservers[attribute]->observeClassAttributeValue(classLabel, value, weight);
}

bool SimpleNaiveBayesStatistics::exportToJson(Json::Value& jv)
{
	jv["sumWeight"] = m_sumWeight;

	for (int i : m_classCounts)
	{
		jv["classCounts"].append(i);
	}

	for (AttributeClassObserver* p : m_attributeClassObservers)
	{
		Json::Value acoJson;
		p->exportToJson(acoJson);

		jv["attrClassOb"].append(acoJson);
	}

	return true;
}

bool SimpleNaiveBayesStatistics::importFromJson(const Json::Value& jv)
{
	m_sumWeight = jv["sumWeight"].asInt();

	LOG_DEBUG("m_sumWeight = %d", m_sumWeight);

	int iClsSize = jv["classCounts"].size();
	m_classCounts.resize(iClsSize);

	for (int i = 0; i < iClsSize; ++i)
	{
		m_classCounts[i] = jv["classCounts"][i].asInt();
	}

	int iObSize = jv["attrClassOb"].size();
	m_attributeClassObservers.resize(iObSize);

	for (int j = 0; j < iObSize; ++j)
	{
		Json::Value obJson = jv["attrClassOb"][j];

		if ("Numeric" == obJson["name"].asString())
		{
			m_attributeClassObservers[j] = new NumericAttributeClassObserver();

		}
		else
		{
			m_attributeClassObservers[j] = new NominalAttributeClassObserver();
		}

		m_attributeClassObservers[j]->importFromJson(obJson);
	}

	return true;
}

