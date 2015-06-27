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

#include "DoubleVector.h"
#include "../Common.h"
DoubleVector::DoubleVector()
{
}

DoubleVector::~DoubleVector()
{
    m_dataVec.clear();
}

//////////////////////////////////////////////////////////////////////////

void DoubleVector::addToValue(int index, double val)
{
	if (index < 0) {
		return;
	}

    if (index >= (int)m_dataVec.size())
    {
        m_dataVec.resize(index+1, 0.0);
    }

    m_dataVec[index] += val;
}

void DoubleVector::setValue(int index, double val)
{
	if (index < 0) {
		return;
	}

    if (index >= (int)m_dataVec.size())
    {
        m_dataVec.resize(index+1, 0.0);
    }

    m_dataVec[index] = val;
}

// returns 0.0 for values outside of range
double DoubleVector::getValue(int index) 
{
    return ((index >= 0) && (index < (int)m_dataVec.size())) ? m_dataVec[index] : 0.0;
}

double DoubleVector::sumOfValues() 
{
    double sum = 0.0;

    for (vector<double>::const_iterator it = m_dataVec.begin(); it != m_dataVec.end(); ++it) 
    {
        sum += *it;
    }

    return sum;
}

int DoubleVector::numValues() 
{
    return (int)m_dataVec.size();
}

bool DoubleVector::exportToJson(Json::Value& jv)
{
	for(double i : m_dataVec)
	{
		jv["DV"].append(i);
	}

	return true;
}

bool DoubleVector::importFromJson(const Json::Value& jv)
{
	int iSize = jv.size();
	m_dataVec.resize(iSize);

	for(int i = 0; i < iSize; ++i)
	{
		m_dataVec[i] = jv["DV"][i].asDouble();
	}

	return true;
}
