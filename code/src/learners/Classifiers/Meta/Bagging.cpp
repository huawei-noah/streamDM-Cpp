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


#include "Bagging.h"
#include "../../../Common.h"

REGISTER_CLASS(Bagging) ;
REGISTER_COMMAND_LINE_PARAMETER(
		Bagging,
		"{\"type\":\"Ensemble\","
		"\"name\":\"Bagging\","
		"\"parameter\":{"
		"\"-l\":\"Learner\","
		"\"-es\":\"EnsembleSize\""
		"}}"
		"");

Bagging::Bagging():
		numberClasses(0),
		mEnsembleSize(0)
{
	init = false;
	instancesSeen = 0;
}

Bagging::~Bagging() {
}

void Bagging::doSetParams() {
	mEnsembleSize = getParam("EnsembleSize", 10);
	Json::Value jv = getParam("Learner");
	if (! jv.isNull()) {
		mLearnerName = jv["Name"].asString();
		mLearnerParams = jv.toStyledString();
	}
}

void Bagging::train(const Instance& instance) {
	if (!init) {
		init = true;
		for (int i = 0; i < mEnsembleSize; i++) {
			learners.push_back(newLearner());
		}
		instancesSeen = 0;
		numberClasses = instance.getNumberClasses();
	}

	for (int i = 0; i < mEnsembleSize; i++) {
		learners[i]->trainBagging(instance);
	}
}

bool Bagging::exportToJson(Json::Value& jv) {
	if (!init) {
		return false;
	}

	jv["nClasses"] = numberClasses;
	jv["ensSize"] = mEnsembleSize;
	jv["learnerName"] = mLearnerName;

	for (int i = 0; i < mEnsembleSize; i++) {
		Json::Value learnerjv;
		ostringstream str;
		learners[i]->exportToJson(learnerjv);
		str << "learner" << i;
		jv[str.str()] = learnerjv;
	}

	return true;
}

bool Bagging::importFromJson(const Json::Value& jv) {
	if (init) {
		return false;
	}

	numberClasses = jv["nClasses"].asInt();
	mEnsembleSize = jv["ensSize"].asInt();
	mLearnerName = jv["learnerName"].asString();

	for (int i = 0; i < mEnsembleSize; i++) {
		ostringstream str;
		str << "learner" << i;
		learners.push_back(newLearner());
		learners[i]->importFromJson(jv[str.str()]);
	}

	init = true;
	return true;
}

double* Bagging::getPrediction(const Instance& instance) {
	int numberClasses = instance.getNumberClasses();

	double* classPrediction = new double[numberClasses];
	for (int j = 0; j < numberClasses; j++) {
		classPrediction[j] = 0.0;
	}

	// statistics all learner's predict in map
	ostringstream strLog;

	std::map<int, int> mapClassID2Count;
	for (int i = 0; i < mEnsembleSize; i++) {
		int predict =  learners[i]->predict(instance);

		auto itr = mapClassID2Count.find(predict);
		if (itr == mapClassID2Count.end()) {
			mapClassID2Count[predict] = 1;
		}
		else {
			itr->second += 1;
		}
	}

	// get the real predict with the biggest count of all prediction
	int index = 0;
	int maxCount = 0;
	for (auto it : mapClassID2Count) {
		if (maxCount < it.second) {
			maxCount = it.second;
			index = it.first;
		}
	}

	classPrediction[index] = 1.0;

	return classPrediction;
}

string Bagging::getEnsemblePrediction(const Instance& instance) {
	// value+1 is used for online_cm which is not support value 0
	ostringstream strPred;
	strPred << learners[0]->predict(instance)+1;

	for (int i = 1; i < mEnsembleSize; i++) {
		strPred << "," << learners[i]->predict(instance)+1;
	}

	return strPred.str();
}

Learner* Bagging::newLearner() {
	Learner* learner = (Learner*)CREATE_CLASS(mLearnerName) ;
	if (mLearnerParams != "") {
		learner->setParams(mLearnerParams);
	}
	return learner;
}
