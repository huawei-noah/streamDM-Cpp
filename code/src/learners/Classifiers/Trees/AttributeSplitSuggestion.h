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

#ifndef ATTRIBUTESPLITSUGGESTION_H_
#define ATTRIBUTESPLITSUGGESTION_H_

#include <vector>
#include "InstanceConditionalTest.h"
using namespace std;

namespace HT {

class AttributeSplitSuggestion {
public:
	AttributeSplitSuggestion(InstanceConditionalTest* splitTest,
	        vector<vector<double>>& resultingClassDistributions, double merit);
	virtual ~AttributeSplitSuggestion();

	InstanceConditionalTest* splitTest;
	vector<vector<double>>* resultingClassDistributions;
	double merit;
	int numSplits();
	vector<double>* resultingClassDistributionFromSplit(int splitIndex);
	int compareTo(AttributeSplitSuggestion* comp);

};

} /* namespace HT */

#endif /* ATTRIBUTESPLITSUGGESTION_H_ */
