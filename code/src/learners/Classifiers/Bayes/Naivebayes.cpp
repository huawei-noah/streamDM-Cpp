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

#include "Naivebayes.h"
#include "SimpleNaiveBayesStatistics.h"
#include "../../../Common.h"

REGISTER_CLASS(NaiveBayes);
REGISTER_COMMAND_LINE_PARAMETER(
		NaiveBayes,
		"{\"type\":\"Learner\","
		"\"name\":\"NaiveBayes\","
		"\"parameter\":{"
		"}}"
		"");


//Naive Bayes Classifier
NaiveBayes::NaiveBayes():
		nbStatistics(nullptr)
{
    init = false;
	//instancesCorrectlyClassified = 0;
	instancesSeen = 0;
}

NaiveBayes::~NaiveBayes() 
{
	if (init) 
    {
		delete nbStatistics;
	}
}

void NaiveBayes::doSetParams() {
	//
}

void NaiveBayes::train(const Instance& instance) 
{
	if (!init) 
    {
        init = true;
		nbStatistics = new SimpleNaiveBayesStatistics();
	}

	int label = (int) instance.getLabel();
	double weight = instance.getWeight();

	nbStatistics->addObservation(label, weight);
	for (int j = 0; j < instance.getNumberInputAttributes(); j++) 
    {
		double value = instance.getInputAttributeValue(j);
        bool isAttributeNumeric = instance.getInputAttribute(j)->isNumeric();
		nbStatistics->addObservation(label, j, isAttributeNumeric, value, weight);
	}
}

double* NaiveBayes::getPrediction(const Instance& instance) {
	double* classPrediction = new double[instance.getNumberClasses()];
	for (int i = 0; i < instance.getNumberClasses(); i++) {
		classPrediction[i] = probability(instance, i);
	}
	return classPrediction;
}

double NaiveBayes::probability(const Instance& instance, int label) {
	if (!init)
	{
		return 0.0;
	}

	double prob = this->nbStatistics->probabilityOfClass(label);
	for (int j = 0; j < instance.getNumberInputAttributes(); j++) {
		double value = instance.getInputAttributeValue(j);
		prob *= this->nbStatistics->probabilityOfClassAttrValue(label, j,
				value);
	}
	return prob;
}

bool NaiveBayes::exportToJson(Json::Value& jv)
{
	if (init)
	{
		this->nbStatistics->exportToJson(jv);
	}

	return true;
}

bool NaiveBayes::importFromJson(const Json::Value& jv)
{
	if (!init)
	{
		this->nbStatistics = new SimpleNaiveBayesStatistics();
		this->nbStatistics->importFromJson(jv);
		init = true;
	}

	return true;
}



