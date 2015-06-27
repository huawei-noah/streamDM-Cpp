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

#ifndef HATNODE_H_
#define HATNODE_H_

#include <list>
#include <vector>
using namespace std;

#include "HTNode.h"
#include "ADWIN.h"
#include "InstanceConditionalTest.h"

namespace HT {

class HoeffdingAdaptiveTree;

class NewNode {
public:
	NewNode();
	virtual ~NewNode();
	virtual int numberLeaves() = 0;
	virtual double getErrorEstimation() = 0;
	virtual double getErrorWidth() = 0;
	virtual bool isNullError() = 0;
	virtual void killTreeChilds(HoeffdingAdaptiveTree* ht) = 0;
	virtual void learnFromInstance(const Instance* inst,
			HoeffdingAdaptiveTree* ht, SplitNode* parent, int parentBranch) = 0;
	virtual void filterInstanceToLeaves(const Instance* inst,
			SplitNode* myparent, int parentBranch, list<FoundNode*>*& foundNodes,
			bool updateSplitterCounts) = 0;
};

NewNode* toNewNode(Node* node);

class AdaSplitNode: public SplitNode, public NewNode {
public:
	Node* alternateTree;
	ADWIN* estimationErrorWeight;
	bool ErrorChange;

	AdaSplitNode(InstanceConditionalTest* splitTest,
			const vector<double>& classObservations, int size);
	AdaSplitNode(InstanceConditionalTest* splitTest,
			const vector<double>& classObservations);
	AdaSplitNode(const Json::Value& jv);
	int numberLeaves();
	double getErrorEstimation();
	double getErrorWidth();
	bool isNullError();
	virtual void learnFromInstance(const Instance* inst, HoeffdingAdaptiveTree* ht,
			SplitNode* parent, int parentBranch);
	void killTreeChilds(HoeffdingAdaptiveTree* ht);
	void filterInstanceToLeaves(const Instance* inst, SplitNode* myparent,
			int parentBranch, list<FoundNode*>*& foundNodes,
			bool updateSplitterCounts);
	virtual void toJson(Json::Value& jv);
};

class AdaLearningNode: public LearningNodeNBAdaptive, public NewNode {
public:
	ADWIN* estimationErrorWeight;
	bool ErrorChange;

	AdaLearningNode(const vector<double>& initialClassObservations);
	AdaLearningNode(const Json::Value& jv);
	int numberLeaves();
	double getErrorEstimation();
	double getErrorWidth();
	bool isNullError();
	void killTreeChilds(HoeffdingAdaptiveTree* ht);
	virtual void learnFromInstance(const Instance* inst, HoeffdingAdaptiveTree* ht,
			SplitNode* parent, int parentBranch);
	vector<double>* getClassVotes(const Instance* inst, HoeffdingTree* ht);
	void filterInstanceToLeaves(const Instance* inst, SplitNode* splitparent,
			int parentBranch, list<FoundNode*>*& foundNodes,
			bool updateSplitterCounts);
	virtual void toJson(Json::Value& jv);
};

} /* namespace HT */

#endif /* HATNODE_H_ */
