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

#ifndef HOEFFDINGADAPTIVETREE_H_
#define HOEFFDINGADAPTIVETREE_H_

#include <vector>
#include <list>
#include "HoeffdingTree.h"
#include "InstanceConditionalTest.h"
using namespace std;

namespace HT {

extern bool debugFlag;
extern int instance_count2;

class HoeffdingAdaptiveTree: public HoeffdingTree {
public:
	HoeffdingAdaptiveTree();
	virtual ~HoeffdingAdaptiveTree();

	int alternateTrees;
	int prunedAlternateTrees;
	int switchedAlternateTrees;

	LearningNode* newLearningNode(
			const vector<double>& initialClassObservations);
	SplitNode* newSplitNode(InstanceConditionalTest* splitTest,
			const vector<double>& classObservations);
	SplitNode* newSplitNode(InstanceConditionalTest* splitTest,
			const vector<double>& classObservations, int size);
	void trainOnInstanceImpl(const Instance* inst);
	list<FoundNode*>* filterInstanceToLeaves(const Instance* inst,
			SplitNode* parent, int parentBranch, bool updateSplitterCounts);
	vector<double>* getVotesForInstance(const Instance* inst);

	virtual void toJson(Json::Value& jv);

protected:
	virtual bool importFromJson(const Json::Value& jv);
	virtual bool exportToJson(Json::Value& jv);
};

} /* namespace HT */

#endif /* HOEFFDINGADAPTIVETREE_H_ */
