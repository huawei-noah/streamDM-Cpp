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

#ifndef HTATTRIBUTECLASSOBSERVER_H_
#define HTATTRIBUTECLASSOBSERVER_H_

#include <vector>
#include <list>
#include "AttributeSplitSuggestion.h"
#include "../../../Common.h"
using namespace std;

namespace HT {

class AttributeClassObserver {
public:
	AttributeClassObserver();
	virtual ~AttributeClassObserver();
	virtual void observeAttributeClass(double attVal, int classVal,
			double weight) = 0;
	virtual double probabilityOfAttributeValueGivenClass(double attVal,
			int classVal) = 0;
	virtual AttributeSplitSuggestion* getBestEvaluatedSplitSuggestion(
			SplitCriterion* criterion, vector<double>& preSplitDist,
			int attIndex, bool binaryOnly) = 0;
	virtual void observeAttributeTarget(double attVal, double target) = 0;
	virtual void toJson(Json::Value& jv) = 0;
};

class NullAttributeClassObserver: public AttributeClassObserver {
public:
	void observeAttributeClass(double attVal, int classVal, double weight);
	double probabilityOfAttributeValueGivenClass(double attVal, int classVal);
	double totalWeightOfClassObservations();
	double weightOfObservedMissingValues();
	AttributeSplitSuggestion* getBestEvaluatedSplitSuggestion(
				SplitCriterion* criterion, vector<double>& preSplitDist,
				int attIndex, bool binaryOnly);
	void observeAttributeTarget(double attVal, double target);
	virtual void toJson(Json::Value& jv);
};

class NumericAttributeClassObserver: public AttributeClassObserver {
};

class GaussianNumericAttributeClassObserver: public NumericAttributeClassObserver {
public:
//    DoubleVector minValueObservedPerClass = new DoubleVector();
//    DoubleVector maxValueObservedPerClass = new DoubleVector();
	vector<double> minValueObservedPerClass;
	vector<double> maxValueObservedPerClass;
	vector<GaussianEstimator*> attValDistPerClass;
	int numBins; // "numBins", 'n',  "The number of bins.", 10, 1, Integer.MAX_VALUE);
	GaussianNumericAttributeClassObserver();
	GaussianNumericAttributeClassObserver(const Json::Value& jv);
	void observeAttributeClass(double attVal, int classVal, double weight);
	double probabilityOfAttributeValueGivenClass(double attVal, int classVal);
	AttributeSplitSuggestion* getBestEvaluatedSplitSuggestion(
			SplitCriterion* criterion, vector<double>& preSplitDist,
			int attIndex, bool binaryOnly);
	list<double>* getSplitPointSuggestions();
	// assume all values equal to splitValue go to lhs
	vector<vector<double>>* getClassDistsResultingFromBinarySplit(
			double splitValue);
	void observeAttributeTarget(double attVal, double target);
	virtual void toJson(Json::Value& jv);
};

class DiscreteAttributeClassObserver: public AttributeClassObserver {
};

class NominalAttributeClassObserver: public DiscreteAttributeClassObserver {
public:
	double totalWeightObserved;
	double missingWeightObserved;
//	list<vector<double>> attValDistPerClass;
	// TODO maybe change list to map can improve the performance
	list<list<double>> attValDistPerClass;
	 vector<int> distSize;
	NominalAttributeClassObserver();
	NominalAttributeClassObserver(const Json::Value& jv);
	void observeAttributeClass(double attVal, int classVal, double weight);
	double probabilityOfAttributeValueGivenClass(double attVal, int classVal);
	double totalWeightOfClassObservations();
	double weightOfObservedMissingValues();

	AttributeSplitSuggestion* getBestEvaluatedSplitSuggestion(
				SplitCriterion* criterion, vector<double>& preSplitDist,
				int attIndex, bool binaryOnly);

	int getMaxAttValsObserved();
	vector<vector<double>>* getClassDistsResultingFromMultiwaySplit(
			int maxAttValsObserved);
	vector<vector<double>>* getClassDistsResultingFromBinarySplit(int valIndex);
	void observeAttributeTarget(double attVal, double target);
	virtual void toJson(Json::Value& jv);
};

} /* namespace HT */

#endif /* HTATTRIBUTECLASSOBSERVER_H_ */
