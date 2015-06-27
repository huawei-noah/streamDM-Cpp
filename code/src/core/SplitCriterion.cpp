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


#include "SplitCriterion.h"
#include <cmath>

namespace HT {

SplitCriterion::SplitCriterion() {
}

SplitCriterion::~SplitCriterion() {
}

InfoGainSplitCriterion::InfoGainSplitCriterion() {
	this->minBranchFrac = 0.01;
}

double InfoGainSplitCriterion::getMeritOfSplit(
		const vector<double>& preSplitDist,
		const vector<vector<double> >& postSplitDists) {
	if (numSubsetsGreaterThanFrac(postSplitDists,
			this->minBranchFrac) < 2) {
		return -1*INFINITY;
	}
	return computeEntropy(preSplitDist) - computeEntropy(postSplitDists);
}

double InfoGainSplitCriterion::getRangeOfMerit(
		const vector<double>& preSplitDist) {
	int numClasses = preSplitDist.size() > 2 ? preSplitDist.size() : 2;
	return log2(numClasses);
}

double InfoGainSplitCriterion::computeEntropy(const vector<double>& dist) {
	double entropy = 0.0;
	double sum = 0.0;
	for (double d : dist) {
		if (d > 0.0) { // TODO: how small can d be before log2 overflows?
			entropy -= d * log2(d);
			sum += d;
		}
	}
	return sum > 0.0 ? (entropy + sum * log2(sum)) / sum : 0.0;

}

double InfoGainSplitCriterion::computeEntropy(
		const vector<vector<double> >& dists) {
	double totalWeight = 0.0;
	vector<double> distWeights(dists.size(), 0);
	for (unsigned int i = 0; i < dists.size(); i++) {
		for (double d : dists[i]) {
			distWeights[i] += d;
		}
		totalWeight += distWeights[i];
	}
	double entropy = 0.0;
	for (unsigned int i = 0; i < dists.size(); i++) {
		entropy += distWeights[i] * computeEntropy(dists[i]);
	}
	return entropy / totalWeight;
}

int InfoGainSplitCriterion::numSubsetsGreaterThanFrac(
		const vector<vector<double> >& distributions, double minFrac) {
	double totalWeight = 0.0;
	vector<double> distSums(distributions.size(), 0);
	for (unsigned int i = 0; i < distSums.size(); i++) {
		for (unsigned int j = 0; j < distributions[i].size(); j++) {
			distSums[i] += distributions[i][j];
		}
		totalWeight += distSums[i];
	}
	int numGreater = 0;
	for (double d : distSums) {
		double frac = d / totalWeight;
		if (frac > minFrac) {
			numGreater++;
		}
	}
	return numGreater;
}

double InfoGainSplitCriterion::log2(double value) {
	return log(value)/log(2.0);
}

} /* namespace HT */
