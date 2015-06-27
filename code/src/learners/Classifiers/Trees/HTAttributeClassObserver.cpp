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

#include "HTAttributeClassObserver.h"
#include <cmath>

namespace HT {

AttributeClassObserver::AttributeClassObserver() {
}

AttributeClassObserver::~AttributeClassObserver() {
}

void NullAttributeClassObserver::observeAttributeClass(double attVal,
		int classVal, double weight) {
}

double NullAttributeClassObserver::probabilityOfAttributeValueGivenClass(
		double attVal, int classVal) {
	return 0.0f;
}

double NullAttributeClassObserver::totalWeightOfClassObservations() {
	return 0.0f;
}

double NullAttributeClassObserver::weightOfObservedMissingValues() {
	return 0.0f;
}

AttributeSplitSuggestion* NullAttributeClassObserver::getBestEvaluatedSplitSuggestion(
		SplitCriterion* criterion, vector<double>& preSplitDist, int attIndex,
		bool binaryOnly) {
	return nullptr;
}

void NullAttributeClassObserver::observeAttributeTarget(double attVal,
		double target) {
}

void NullAttributeClassObserver::toJson(Json::Value& jv) {
	jv["type"] = "NullAttributeClassObserver";
}

GaussianNumericAttributeClassObserver::GaussianNumericAttributeClassObserver() {
	this->numBins = 10;
}

void GaussianNumericAttributeClassObserver::observeAttributeClass(double attVal,
		int classVal, double weight) {
	if (!std::isnan(attVal)) {
		GaussianEstimator* valDist;
		if (classVal >= attValDistPerClass.size()) {
			attValDistPerClass.resize(classVal + 1, nullptr);
			minValueObservedPerClass.resize(classVal + 1, 0);
			maxValueObservedPerClass.resize(classVal + 1, 0);
		}
		valDist = this->attValDistPerClass[classVal];
		if (valDist == nullptr) {
			valDist = new GaussianEstimator();
			this->attValDistPerClass[classVal] = valDist;
			this->minValueObservedPerClass[classVal] = attVal;
			this->maxValueObservedPerClass[classVal] = attVal;
		} else {
			if (attVal < this->minValueObservedPerClass[classVal]) {
				this->minValueObservedPerClass[classVal] = attVal;
			}
			if (attVal > this->maxValueObservedPerClass[classVal]) {
				this->maxValueObservedPerClass[classVal] = attVal;
			}
		}
		valDist->addObservation(attVal, weight);
	}
}

double GaussianNumericAttributeClassObserver::probabilityOfAttributeValueGivenClass(
		double attVal, int classVal) {
	if (classVal >= (double) (this->attValDistPerClass.size())) {
		return 0.0f;
	}
	GaussianEstimator* obs = this->attValDistPerClass[classVal];
	return obs != nullptr ? obs->probabilityDensity(attVal) : 0.0f;
}

AttributeSplitSuggestion* GaussianNumericAttributeClassObserver::getBestEvaluatedSplitSuggestion(
		SplitCriterion* criterion, vector<double>& preSplitDist, int attIndex,
		bool binaryOnly) {
	AttributeSplitSuggestion* bestSuggestion = nullptr;
	list<double>* suggestedSplitValues = getSplitPointSuggestions();

	for (double splitValue : *suggestedSplitValues) {
		vector<vector<double>>* postSplitDists =
				getClassDistsResultingFromBinarySplit(splitValue);
		double merit = criterion->getMeritOfSplit(preSplitDist,
				*postSplitDists);
		if (bestSuggestion == nullptr) {
			bestSuggestion = new AttributeSplitSuggestion(
					new NumericAttributeBinaryTest(attIndex, splitValue, true),
					*postSplitDists, merit);
		} else if (merit > bestSuggestion->merit) {
			delete bestSuggestion;
			bestSuggestion = new AttributeSplitSuggestion(
					new NumericAttributeBinaryTest(attIndex, splitValue, true),
					*postSplitDists, merit);
		} else {
			delete postSplitDists;
		}
	}
	delete suggestedSplitValues;
	return bestSuggestion;
}

list<double>* GaussianNumericAttributeClassObserver::getSplitPointSuggestions() {
	list<double>* suggestedSplitValues = new list<double>;
	double minValue = INFINITY;
	double maxValue = -1 * INFINITY;
	for (unsigned int i = 0; i < this->attValDistPerClass.size(); i++) {
		GaussianEstimator* estimator = this->attValDistPerClass[i];
		if (estimator != nullptr) {
			if (this->minValueObservedPerClass[i] < minValue) {
				minValue = this->minValueObservedPerClass[i];
			}
			if (this->maxValueObservedPerClass[i] > maxValue) {
				maxValue = this->maxValueObservedPerClass[i];
			}
		}
	}
	if (minValue < INFINITY) {
		double range = maxValue - minValue;
		for (int i = 0; i < this->numBins; i++) {
			double splitValue = range / (this->numBins + 1.0) * (i + 1)
					+ minValue;
			if ((splitValue > minValue) && (splitValue < maxValue)) {
				suggestedSplitValues->push_back(splitValue);
			}
		}
	}
	return suggestedSplitValues;
}

vector<vector<double> >* GaussianNumericAttributeClassObserver::getClassDistsResultingFromBinarySplit(
		double splitValue) {
	vector<vector<double>>* dist = new vector<vector<double>>(2);
	for (unsigned int i = 0; i < this->attValDistPerClass.size(); i++) {
		GaussianEstimator* estimator = this->attValDistPerClass[i];
		if (estimator != nullptr) {
			if (splitValue < this->minValueObservedPerClass[i]) {
				Utils::addToValue((*dist)[1], i,
						estimator->getTotalWeightObserved());
			} else if (splitValue >= this->maxValueObservedPerClass[i]) {
				Utils::addToValue((*dist)[0], i,
						estimator->getTotalWeightObserved());
			} else {
				vector<double>* weightDist =
						estimator->estimatedWeight_LessThan_EqualTo_GreaterThan_Value(
								splitValue);
				Utils::addToValue((*dist)[0], i,
						(*weightDist)[0] + (*weightDist)[1]);
				Utils::addToValue((*dist)[1], i, (*weightDist)[2]);
				delete weightDist;
			}
		}
	}
	return dist;
}

void GaussianNumericAttributeClassObserver::observeAttributeTarget(
		double attVal, double target) {
//	throw new UnsupportedOperationException("Not supported yet.");
}

GaussianNumericAttributeClassObserver::GaussianNumericAttributeClassObserver(
		const Json::Value& jv) {
	int arraySize = jv["minValueObservedPerClass"].size();
	this->minValueObservedPerClass.resize(arraySize, 0);
	for (int i = 0; i < arraySize; i++) {
		this->minValueObservedPerClass[i] =
				jv["minValueObservedPerClass"][i].asDouble();
	}

	arraySize = jv["maxValueObservedPerClass"].size();
	this->maxValueObservedPerClass.resize(arraySize, 0);
	for (int i = 0; i < arraySize; i++) {
		this->maxValueObservedPerClass[i] =
				jv["maxValueObservedPerClass"][i].asDouble();
	}

	arraySize = jv["attValDistPerClass"].size();
	this->attValDistPerClass.resize(arraySize, nullptr);
	for (int i = 0; i < arraySize; i++) {
		if (jv["attValDistPerClass"][i].isObject()) {
			this->attValDistPerClass[i] = new GaussianEstimator(
					jv["attValDistPerClass"][i]);
		}
	}

	this->numBins = jv["numBins"].asInt();
}

void GaussianNumericAttributeClassObserver::toJson(Json::Value& jv) {
	jv["type"] = "GaussianNumericAttributeClassObserver";
	int i = 0;
	for (double d : minValueObservedPerClass) {
		jv["minValueObservedPerClass"][i] = d;
		i++;
	}
	i = 0;
	for (double d : maxValueObservedPerClass) {
		jv["maxValueObservedPerClass"][i] = d;
		i++;
	}
	i = 0;
	for (auto iter = attValDistPerClass.begin();
			iter != attValDistPerClass.end(); iter++, i++) {
		if ((*iter) == nullptr) {
			jv["attValDistPerClass"][i] = "nullptr";
		} else {
			(*iter)->toJson(jv["attValDistPerClass"][i]);
		}
	}
	jv["numBins"] = this->numBins;
}

NominalAttributeClassObserver::NominalAttributeClassObserver() {
	this->totalWeightObserved = 0.0;
	this->missingWeightObserved = 0.0;
}

NominalAttributeClassObserver::NominalAttributeClassObserver(
		const Json::Value& jv) {
	this->totalWeightObserved = jv["totalWeightObserved"].asDouble();
	this->missingWeightObserved = jv["missingWeightObserved"].asDouble();

	int i = 0;
	this->attValDistPerClass.resize(jv["attValDistPerClass"].size());
	this->distSize.resize(jv["attValDistPerClass"].size());
	for (auto iter = this->attValDistPerClass.begin();
			iter != this->attValDistPerClass.end(); iter++, i++) {
		(*iter).resize(jv["attValDistPerClass"][i].size());
		int j = 0;
		for (auto iter2 = (*iter).begin(); iter2 != (*iter).end();
				iter2++, j++) {
			(*iter2) = jv["attValDistPerClass"][i][j].asDouble();
		}
		this->distSize[i] = jv["distSize"][i].asInt();
	}
}

void NominalAttributeClassObserver::toJson(Json::Value& jv) {
	jv["type"] = "NominalAttributeClassObserver";
	jv["totalWeightObserved"] = this->totalWeightObserved;
	jv["missingWeightObserved"] = this->missingWeightObserved;
	int i = 0;
	for (auto l1 : attValDistPerClass) {
		int j = 0;
		for (auto l2 : l1) {
			jv["attValDistPerClass"][i][j] = l2;
			j++;
		}
		jv["distSize"][i] = distSize[i];
		i++;
	}
}

void NominalAttributeClassObserver::observeAttributeClass(double attVal,
		int classVal, double weight) {
	if (std::isnan(attVal)) {
		this->missingWeightObserved += weight;
	} else {
		int attValInt = (int) attVal;

		auto iter = attValDistPerClass.begin();
		if (classVal >= (int) (attValDistPerClass.size())) {
			attValDistPerClass.resize(classVal + 1);
			distSize.resize(classVal + 1, 0);
			iter = attValDistPerClass.end();
			iter--;
		} else {
			advance(iter, classVal);
		}

		list<double> &attr = (*iter);
		if (attValInt >= (int) (attr.size())) {
			attr.resize(attValInt + 1, 0);
			distSize[classVal] = attValInt + 1;
			auto iterAttr = attr.end();
			iterAttr--;
			(*iterAttr) += weight;
		} else {
			auto iterAttr = attr.begin();
			advance(iterAttr, attValInt);
			(*iterAttr) += weight;
		}
	}
	this->totalWeightObserved += weight;
}

double NominalAttributeClassObserver::probabilityOfAttributeValueGivenClass(
		double attVal, int classVal) {

	if (classVal >= (int) (this->attValDistPerClass.size())) {
		return 0.0f;
	}

	auto iter = this->attValDistPerClass.begin();
	advance(iter, classVal);
	auto &attr = (*iter);

	double arraySize = attr.size();
	if (arraySize == 0) {
		return 0.0f;
	}
	double sumOfValues = Utils::sum(attr);
	double savedAttrValue = 0.0f;
	if (attVal < arraySize) {
		auto iterAttr = attr.begin();
		advance(iterAttr, (int) attVal);
		savedAttrValue = (*iterAttr);
	}

	return (savedAttrValue + 1.0) / (sumOfValues + arraySize);
}

double NominalAttributeClassObserver::totalWeightOfClassObservations() {
	return this->totalWeightObserved;
}

double NominalAttributeClassObserver::weightOfObservedMissingValues() {
	return this->missingWeightObserved;
}

AttributeSplitSuggestion* NominalAttributeClassObserver::getBestEvaluatedSplitSuggestion(
		SplitCriterion* criterion, vector<double>& preSplitDist, int attIndex,
		bool binaryOnly) {
	AttributeSplitSuggestion* bestSuggestion = nullptr;
	int maxAttValsObserved = getMaxAttValsObserved();
	if (!binaryOnly) {
		vector<vector<double>>* postSplitDists =
				getClassDistsResultingFromMultiwaySplit(maxAttValsObserved);
		double merit = criterion->getMeritOfSplit(preSplitDist,
				*postSplitDists);
		bestSuggestion = new AttributeSplitSuggestion(
				new NominalAttributeMultiwayTest(attIndex), *postSplitDists,
				merit);
	}
	for (int valIndex = 0; valIndex < maxAttValsObserved; valIndex++) {
		vector<vector<double>>* postSplitDists =
				getClassDistsResultingFromBinarySplit(valIndex);
		double merit = criterion->getMeritOfSplit(preSplitDist,
				*postSplitDists);
		if (bestSuggestion == nullptr) {
			bestSuggestion = new AttributeSplitSuggestion(
					new NominalAttributeBinaryTest(attIndex, valIndex),
					*postSplitDists, merit);
		} else if (merit > bestSuggestion->merit) {
			delete bestSuggestion;
			bestSuggestion = new AttributeSplitSuggestion(
					new NominalAttributeBinaryTest(attIndex, valIndex),
					*postSplitDists, merit);
		} else {
			delete postSplitDists;
		}
	}
	return bestSuggestion;
}

int NominalAttributeClassObserver::getMaxAttValsObserved() {
	int maxAttValsObserved = 0;
	for (unsigned int i = 0; i < this->distSize.size(); i++) {
		if (distSize[i] > maxAttValsObserved) {
			maxAttValsObserved = distSize[i];
		}
	}
	return maxAttValsObserved;
}

vector<vector<double> >* NominalAttributeClassObserver::getClassDistsResultingFromMultiwaySplit(
		int maxAttValsObserved) {
	vector<vector<double>>* dist = new vector<vector<double>>;
	auto iter = attValDistPerClass.begin();
	int i = 0;
	for (; iter != attValDistPerClass.end(); i++, iter++) {
		auto iterAttr = (*iter).begin();
		int j = 0;
		for (; iterAttr != (*iter).end(); iterAttr++, j++) {
			if (j >= (int) (dist->size())) {
				dist->resize(j + 1);
			}
			Utils::addToValue((*dist)[j], i, *iterAttr);
		}
	}
	return dist;
}

vector<vector<double> >* NominalAttributeClassObserver::getClassDistsResultingFromBinarySplit(
		int valIndex) {
	// FIXME changing to list can improve the performance
	vector<vector<double>>* dist = new vector<vector<double>>(2);
	int i = 0;
	auto iter = this->attValDistPerClass.begin();
	for (; iter != this->attValDistPerClass.end(); iter++, i++) {
		int j = 0;
		auto iterAttr = (*iter).begin();
		for (; iterAttr != (*iter).end(); iterAttr++, j++) {
			if (j == valIndex) {
				Utils::addToValue((*dist)[0], i, (*iterAttr));
			} else {
				Utils::addToValue((*dist)[1], i, (*iterAttr));
			}
		}
	}
	return dist;
}

void NominalAttributeClassObserver::observeAttributeTarget(double attVal,
		double target) {
//	throw new UnsupportedOperationException("Not supported yet.");
}

} /* namespace HT */
