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

#ifndef EVALUATEHOLDOUT_H_
#define EVALUATEHOLDOUT_H_

#include "../Common.h"

class EvaluateHoldOut: public Task {
public:
	EvaluateHoldOut();
	virtual ~EvaluateHoldOut();
	int doTask();

private:
    int doUnifiedTask(Learner& learner, Reader& reader);

	void doSetParams();
    string mLearnerName;
    string mLearnerParams;
    string mReaderName;
    string mReaderParams;
    string mTrainFile;
    string mTestFile;
    string mOutputModelFile;
    string mEvaluatorName;
    string mEvaluatorParams;
};

#endif /* EVALUATEHOLDOUT_H_ */
