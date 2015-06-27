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

#include "AttributeSplitSuggestion.h"
#include <iostream>

namespace HT {

AttributeSplitSuggestion::AttributeSplitSuggestion(
		InstanceConditionalTest* splitTest,
		vector<vector<double> >& resultingClassDistributions,
		double merit) {
	this->splitTest = splitTest;
	this->resultingClassDistributions = &resultingClassDistributions;
	this->merit = merit;
}

AttributeSplitSuggestion::~AttributeSplitSuggestion() {
	delete resultingClassDistributions;
	delete splitTest;
}

int AttributeSplitSuggestion::numSplits() {
	return resultingClassDistributions->size();
}

vector<double>* AttributeSplitSuggestion::resultingClassDistributionFromSplit(
		int splitIndex) {
	return &((*resultingClassDistributions)[splitIndex]);
}

int AttributeSplitSuggestion::compareTo(AttributeSplitSuggestion* comp) {
	return int(this->merit - comp->merit);

	if (this->merit > comp->merit) {
		return 1;
	} else if (this->merit == comp->merit) {
		return 0;
	} else {
		return -1;
	}
}

} /* namespace HT */
