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

#ifndef NUMERICATTRIBUTECLASSOBSERVER_H_
#define NUMERICATTRIBUTECLASSOBSERVER_H_

#include <vector>
#include "../../../../Common.h"
#include "AttributeClassObserver.h"

class NumericAttributeClassObserver : public AttributeClassObserver
{
public:
    NumericAttributeClassObserver();
    virtual ~NumericAttributeClassObserver();

public:
    virtual void observeClassAttributeValue(int classVal, double attVal, double weight);
    virtual double probabilityOfClassAttrValue(int classVal, double attVal);

    virtual bool exportToJson(Json::Value& jv);
    virtual bool importFromJson(const Json::Value& jv);

protected:
    vector<GaussianEstimator*> m_attValDistPerClass;
    DoubleVector m_minValueObservedPerClass;
    DoubleVector m_maxValueObservedPerClass;
};

#endif /* NUMERICATTRIBUTECLASSOBSERVER_H_ */
