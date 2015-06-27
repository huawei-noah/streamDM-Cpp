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

#ifndef SPLITCRITERION_H_
#define SPLITCRITERION_H_

#include <vector>
using namespace std;

namespace HT {

class SplitCriterion {
public:
	SplitCriterion();
	virtual ~SplitCriterion();
	virtual double getMeritOfSplit(const vector<double>& preSplitDist,
			const vector<vector<double>>& postSplitDists) = 0;
	virtual double getRangeOfMerit(const vector<double>& preSplitDist) = 0;
};

class InfoGainSplitCriterion: public SplitCriterion {
public:
	InfoGainSplitCriterion();
	// TODO option miniBranchFrac
	double minBranchFrac; // "minBranchFrac", 'f',  "Minimum fraction of weight required down at least two branches.", 0.01, 0.0, 0.5);
	double getMeritOfSplit(const vector<double>& preSplitDist,
			const vector<vector<double>>& postSplitDists);
	double getRangeOfMerit(const vector<double>& preSplitDist);
	double computeEntropy(const vector<double>& dist);
	double computeEntropy(const vector<vector<double>>& dists);
	int numSubsetsGreaterThanFrac(const vector<vector<double>>& distributions,
			double minFrac);
	double log2(double value);
};

} /* namespace HT */

#endif /* SPLITCRITERION_H_ */
