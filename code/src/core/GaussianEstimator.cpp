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

#include "GaussianEstimator.h"

#define  _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#include <iostream>

const double GaussianEstimator::NORMAL_CONSTANT = sqrt(2 * M_PI);

#define SQRTH 7.07106781186547524401E-1
#define MAXLOG 7.09782712893383996732E2

GaussianEstimator::GaussianEstimator()
{
	m_weightSum = 0.0;
	m_mean = 0.0;
	m_varianceSum = 0;
}


GaussianEstimator::GaussianEstimator(const Json::Value& jv) {
	m_weightSum = jv["weightSum"].asDouble();
	m_mean = jv["mean"].asDouble() ;
	m_varianceSum = jv["varianceSum"].asDouble();
}

GaussianEstimator::~GaussianEstimator()
{
}

void GaussianEstimator::toJson(Json::Value& jv){
	jv["type"] = "GaussianEstimator";
	jv["mean"] = this->m_mean;
	jv["weightSum"] = this->m_weightSum;
	jv["varianceSum"] = this->m_varianceSum;
}
//////////////////////////////////////////////////////////////////////////

void GaussianEstimator::addObservation(double value, double weight)
{
    // TODO: no judge value valid, maybe need to add judge
    
    if (m_weightSum > 0.0) 
    {
        m_weightSum += weight;
        double lastMean = m_mean;
        m_mean += weight * (value - lastMean) / m_weightSum; 
        m_varianceSum += weight * (value - lastMean) * (value - m_mean);
    } 
    else 
    {
        m_mean = value;
        m_weightSum = weight;
        m_varianceSum = 0;
    }
}

double GaussianEstimator::getStdDev() 
{
    return sqrt(getVariance());
}

double GaussianEstimator::getVariance() 
{
    return (m_weightSum > 1.0) ? (m_varianceSum / (m_weightSum - 1.0)) : 0.0;
}

double GaussianEstimator::getTotalWeightObserved() {
	return this->m_weightSum;
}

double GaussianEstimator::getMean() {
	return this->m_mean;
}

double GaussianEstimator::probabilityDensity(double value) 
{
    if (m_weightSum > 0.0) 
    {
        double stdDev = getStdDev();

        if (stdDev > 0.0) 
        {
            double diff = value - m_mean;
            return (1.0 / (NORMAL_CONSTANT * stdDev))
                * exp(-(diff * diff / (2.0 * stdDev * stdDev)));
        }

        return (value == m_mean) ? 1.0 : 0.0;
    }

    return 0.0;
}

vector<double>* GaussianEstimator::estimatedWeight_LessThan_EqualTo_GreaterThan_Value(double value) {
    double equalToWeight = probabilityDensity(value) * this->m_weightSum;
    double stdDev = getStdDev();
    double lessThanWeight = stdDev > 0.0 ? normalProbability((value - getMean()) / stdDev)
            * this->m_weightSum - equalToWeight
            : (value < getMean() ? this->m_weightSum - equalToWeight : 0.0);
    double greaterThanWeight = this->m_weightSum - equalToWeight
            - lessThanWeight;
    if (greaterThanWeight < 0.0) {
        greaterThanWeight = 0.0;
    }
    vector<double>* retVec = new vector<double>{lessThanWeight, equalToWeight, greaterThanWeight};
    return retVec;
}

double GaussianEstimator::normalProbability(double a) {
    double x, y, z;

    x = a * SQRTH;
    z = fabs(x);

    if (z < SQRTH) {
      y = 0.5 + 0.5 * errorFunction(x);
    } else {
      y = 0.5 * errorFunctionComplemented(z);
      if (x > 0) {
        y = 1.0 - y;
      }
    }
    return y;
}

double GaussianEstimator::errorFunction(double x) {
    double y, z;
    static double T[] = { 9.60497373987051638749E0, 9.00260197203842689217E1,
      2.23200534594684319226E3, 7.00332514112805075473E3,
      5.55923013010394962768E4 };
    static double U[] = {
      // 1.00000000000000000000E0,
      3.35617141647503099647E1, 5.21357949780152679795E2,
      4.59432382970980127987E3, 2.26290000613890934246E4,
      4.92673942608635921086E4 };

    if (fabs(x) > 1.0) {
      return (1.0 - errorFunctionComplemented(x));
    }
    z = x * x;
    y = x * polevl(z, T, 4) / p1evl(z, U, 5);
    return y;
}

double GaussianEstimator::polevl(double x, double* coef, int N) {
    double ans;
    ans = coef[0];

    for (int i = 1; i <= N; i++) {
      ans = ans * x + coef[i];
    }

    return ans;
}

double GaussianEstimator::p1evl(double x, double* coef, int N) {
    double ans;
    ans = x + coef[0];

    for (int i = 1; i < N; i++) {
      ans = ans * x + coef[i];
    }

    return ans;
}

double GaussianEstimator::errorFunctionComplemented(double a) {
	  double x, y, z, p, q;

	    double P[] = { 2.46196981473530512524E-10, 5.64189564831068821977E-1,
	      7.46321056442269912687E0, 4.86371970985681366614E1,
	      1.96520832956077098242E2, 5.26445194995477358631E2,
	      9.34528527171957607540E2, 1.02755188689515710272E3,
	      5.57535335369399327526E2 };
	    double Q[] = {
	      // 1.0
	      1.32281951154744992508E1, 8.67072140885989742329E1,
	      3.54937778887819891062E2, 9.75708501743205489753E2,
	      1.82390916687909736289E3, 2.24633760818710981792E3,
	      1.65666309194161350182E3, 5.57535340817727675546E2 };

	    double R[] = { 5.64189583547755073984E-1, 1.27536670759978104416E0,
	      5.01905042251180477414E0, 6.16021097993053585195E0,
	      7.40974269950448939160E0, 2.97886665372100240670E0 };
	    double S[] = {
	      // 1.00000000000000000000E0,
	      2.26052863220117276590E0, 9.39603524938001434673E0,
	      1.20489539808096656605E1, 1.70814450747565897222E1,
	      9.60896809063285878198E0, 3.36907645100081516050E0 };

	    if (a < 0.0) {
	      x = -a;
	    } else {
	      x = a;
	    }

	    if (x < 1.0) {
	      return 1.0 - errorFunction(a);
	    }

	    z = -a * a;

	    if (z < -MAXLOG) {
	      if (a < 0) {
	        return (2.0);
	      } else {
	        return (0.0);
	      }
	    }

	    z = exp(z);

	    if (x < 8.0) {
	      p = polevl(x, P, 8);
	      q = p1evl(x, Q, 8);
	    } else {
	      p = polevl(x, R, 5);
	      q = p1evl(x, S, 6);
	    }

	    y = (z * p) / q;

	    if (a < 0) {
	      y = 2.0 - y;
	    }

	    if (y == 0.0) {
	      if (a < 0) {
	        return 2.0;
	      } else {
	        return (0.0);
	      }
	    }
	    return y;
}

bool GaussianEstimator::exportToJson(Json::Value& jv)
{
	this->toJson(jv);
	return true;
}

bool GaussianEstimator::importFromJson(const Json::Value& jv)
{
	this->m_mean = jv["mean"].asDouble();
	this->m_weightSum = jv["weightSum"].asDouble();
	this->m_varianceSum = jv["varianceSum"].asDouble();
	return true;
}

