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

#include "EvaluatePrequential.h"

#define TASK_READFILE_FAILED 1
#define TASK_PARAMETER_FAILED 2
#define TASK_SUCCESS 0

REGISTER_CLASS(EvaluatePrequential)
REGISTER_COMMAND_LINE_PARAMETER(
		EvaluatePrequential,
		"{\"type\":\"Task\","
		"\"name\":\"EvaluatePrequential\","
		"\"parameter\":{"
		"\"-l\":\"Learner\","
		"\"-e\":\"Evaluator\","
		"\"-ds\":\"DataSource\","
		"\"-r\":\"Reader\""
		"}}"
		"");

EvaluatePrequential::EvaluatePrequential() {
}

EvaluatePrequential::~EvaluatePrequential() {
}

void EvaluatePrequential::doSetParams() {
	mDataSource = getParam("DataSource", "");

	Json::Value jv = getParam("Learner");
	if (! jv.isNull()) {
		mLearnerName = jv["Name"].asString();
		mLearnerParams = jv.toStyledString();
	}

	jv = getParam("Evaluator");
	if (! jv.isNull()) {
		mEvaluatorName = jv["Name"].asString();
		mEvaluatorParams = jv.toStyledString();
	}
	else {
		mEvaluatorName = "BasicClassificationEvaluator";
	}

	mReaderName = getParam("Reader", "");
	if (mReaderName == "") {
		jv = getParam("Reader");
		if (! jv.isNull()) {
			mReaderName = jv["Name"].asString();
			mReaderParams = jv.toStyledString();
		}
	}
}

int EvaluatePrequential::doUnifiedTask(Learner& learner, Reader& reader)
{
    if (!reader.setFile(mDataSource)) {
        LOG_ERROR("Failed to open file: %s ", mDataSource.c_str());
        return TASK_READFILE_FAILED;
    }

    int counter = 0;
    Instance* inst;
    while (reader.hasNextInstance()) {
        counter++;
        inst = reader.nextInstance();
        learner.process(*inst);
		delete inst;
    }

    return TASK_SUCCESS;
}

int EvaluatePrequential::doTask()
{
	if (mLearnerName == "" || mLearnerParams == "" ||
			mReaderName == "" || mDataSource == "") {
		LOG_ERROR("EvaluatePrequential need required Parameter, "
				"Learner: %s , Reader: %s, Data source: %s, Learner Parameters: %s .",
				mLearnerName.c_str(), mReaderName.c_str(), mDataSource.c_str(), mLearnerParams.c_str());
		return TASK_PARAMETER_FAILED;
	}

	int returnCode = 0;
    Learner* learner = nullptr;
    Evaluator* eval = nullptr;
    Reader* reader = nullptr;

	// learner
	learner = (Learner*) CREATE_CLASS(mLearnerName);
	if (learner == nullptr) {
		LOG_ERROR("Parameter error, not defined class: %s .", mLearnerName.c_str());
		return TASK_PARAMETER_FAILED;
	}
	if (mLearnerParams != "") {
		learner->setParams(mLearnerParams);
	}
	LOG_INFO( "learner: %s ", mLearnerName.c_str() );

	// evaluator
	eval = (Evaluator*) CREATE_CLASS(mEvaluatorName);
	if (eval == nullptr) {
		LOG_ERROR("Parameter error, not defined class: %s .", mEvaluatorName.c_str());
		delete learner;
		return TASK_PARAMETER_FAILED;
	}
	if (mEvaluatorParams != "") {
		eval->setParams(mEvaluatorParams);
	}
	learner->setEvaluator(eval);

	// reader
	reader = (Reader*) CREATE_CLASS(mReaderName) ;
	if (reader == nullptr) {
		LOG_ERROR("Parameter error, not defined class: %s .", mReaderName.c_str());
		delete learner;
		delete eval;
		return TASK_PARAMETER_FAILED;
	}
	if (mReaderParams != "") {
		reader->setParams(mReaderParams);
	}

	returnCode = doUnifiedTask(*learner, *reader);
     
	if (TASK_SUCCESS == returnCode)
	{
		LOG_DEBUG(eval->toString().c_str());
	}

	delete learner;
	delete eval;
	delete reader;

    return returnCode;
}
