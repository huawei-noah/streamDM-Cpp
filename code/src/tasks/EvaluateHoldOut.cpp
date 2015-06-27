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


#include "EvaluateHoldOut.h"
#include "../evaluation/Evaluator.h"

#define TASK_READFILE_FAILED 1
#define TASK_PARAMETER_FAILED 2
#define TASK_SUCCESS 0

REGISTER_CLASS(EvaluateHoldOut);
REGISTER_COMMAND_LINE_PARAMETER(EvaluateHoldOut, "{\"type\":\"Task\","
		"\"name\":\"EvaluateHoldOut\","
		"\"parameter\":{"
		"\"-l\":\"Learner\","
		"\"-e\":\"Evaluator\","
		"\"-a\":\"TrainFile\","
		"\"-t\":\"TestFile\","
		"\"-mo\":\"OutputModelFile\","
		"\"-r\":\"Reader\""
		"}}"
		"");

EvaluateHoldOut::EvaluateHoldOut() {
}

EvaluateHoldOut::~EvaluateHoldOut() {
}

void EvaluateHoldOut::doSetParams() {
	mTrainFile = getParam("TrainFile", "");
	mTestFile = getParam("TestFile", "");
	this->mOutputModelFile = this->getParam("OutputModelFile", "");

	Json::Value jv = getParam("Learner");
	if (!jv.isNull()) {
		mLearnerName = jv["Name"].asString();
		mLearnerParams = jv.toStyledString();
	}

	jv = getParam("Evaluator");
	if (!jv.isNull()) {
		mEvaluatorName = jv["Name"].asString();
		mEvaluatorParams = jv.toStyledString();
	} else {
		mEvaluatorName = "BasicClassificationEvaluator";
	}

	mReaderName = getParam("Reader", "");
	if (mReaderName == "") {
		jv = getParam("Reader");
		if (!jv.isNull()) {
			mReaderName = jv["Name"].asString();
			mReaderParams = jv.toStyledString();
		}
	}
}

int EvaluateHoldOut::doUnifiedTask(Learner& learner, Reader& reader) {
	//Train
	if (!reader.setFile(mTrainFile)) {
		LOG_ERROR("Failed to open file: %s ", mTrainFile.c_str());
		return TASK_READFILE_FAILED;
	}

	//int counter = 0;
	Instance* inst;
	while (reader.hasNextInstance()) {
		//counter++;
		inst = reader.nextInstance();
		learner.train(*inst);
		delete inst;
	}

	// output model
	if (this->mOutputModelFile != "") {
		LOG_INFO("Output model to file: %s .", this->mOutputModelFile.c_str());
		if (!learner.exportToFile(this->mOutputModelFile)) {
			LOG_ERROR("Failed to export model to file: %s .",
					this->mOutputModelFile.c_str());
			return TASK_READFILE_FAILED;
		}
	}

	//Test
	if (!reader.setFile(mTestFile)) {
		LOG_ERROR("Failed to open file: %s ", mTestFile.c_str());
		return TASK_READFILE_FAILED;
	}

	int counter = 0;
	//Instance* inst;
	while (reader.hasNextInstance()) {
		counter++;
		inst = reader.nextInstance();
		learner.getEvaluator()->addResult(*inst, learner.getPrediction(*inst));
		//learner.process(*inst);
		delete inst;
	}

	return TASK_SUCCESS;
}

int EvaluateHoldOut::doTask() {
	int returnCode = 0;

	Learner* learner = nullptr;
	Evaluator* eval = nullptr;
	Reader* reader = nullptr;

	learner = (Learner*) CREATE_CLASS(mLearnerName);
	if (learner == nullptr) {
		LOG_ERROR("Parameter error, not defined class: %s .",
				mLearnerName.c_str());
		return TASK_PARAMETER_FAILED;
	}
	if (mLearnerParams != "") {
		learner->setParams(mLearnerParams);
	}
	LOG_INFO("learner: %s ", mLearnerName.c_str());

	// evaluator
	eval = (Evaluator*) CREATE_CLASS(mEvaluatorName);
	if (eval == nullptr) {
		LOG_ERROR("Parameter error, not defined class: %s .",
				mEvaluatorName.c_str());
		delete learner;
		return TASK_PARAMETER_FAILED;
	}
	if (mEvaluatorParams != "") {
		eval->setParams(mEvaluatorParams);
	}
	learner->setEvaluator(eval);

	// reader
	reader = (Reader*) CREATE_CLASS(mReaderName);
	if (reader == nullptr) {
		LOG_ERROR("Parameter error, not defined class: %s .",
				mReaderName.c_str());
		delete learner;
		delete eval;
		return TASK_PARAMETER_FAILED;
	}
	if (mReaderParams != "") {
		reader->setParams(mReaderParams);
	}

	returnCode = doUnifiedTask(*learner, *reader);

	if (TASK_SUCCESS == returnCode) {
		LOG_DEBUG(eval->toString());
	}

	delete learner;
	delete eval;
	delete reader;

	return returnCode;
}
