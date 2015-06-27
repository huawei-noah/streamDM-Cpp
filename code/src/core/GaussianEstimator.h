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

#ifndef GAUSSIANESTIMATOR_H_
#define GAUSSIANESTIMATOR_H_

#include <vector>
#include <fstream>
#include <string>
#include "../utils/json.h"

using namespace std;

class GaussianEstimator
{
public:
	GaussianEstimator();
	GaussianEstimator(const Json::Value& jv);
	virtual ~GaussianEstimator();

public:
    void addObservation(double value, double weight);
    double getStdDev();
    double getVariance();
    double getTotalWeightObserved();
    double getMean();
    double probabilityDensity(double value);
    vector<double>* estimatedWeight_LessThan_EqualTo_GreaterThan_Value(
                double value);
    void toJson(Json::Value& jv);

    bool exportToJson(Json::Value& jv);
    bool importFromJson(const Json::Value& jv);

public:
    static const double NORMAL_CONSTANT;

public:
    double m_weightSum;
    double m_mean;
    double m_varianceSum;

private:
    double normalProbability(double a);
    double errorFunction(double x);
    double polevl(double x, double* coef, int N);
    double p1evl(double x, double* coef, int N);
    double errorFunctionComplemented(double a);
};

#endif /* GAUSSIANESTIMATOR_H_ */
