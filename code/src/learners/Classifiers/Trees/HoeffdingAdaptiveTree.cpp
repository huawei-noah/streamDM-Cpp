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


#include "HoeffdingAdaptiveTree.h"
#include "HTNode.h"
#include "HATNode.h"

namespace HT {

REGISTER_CLASS(HoeffdingAdaptiveTree)
REGISTER_COMMAND_LINE_PARAMETER(HoeffdingAdaptiveTree, "{\"type\":\"Learner\","
		"\"name\":\"HoeffdingAdaptiveTree\","
		"\"parameter\":{"
		"\"-m\":\"MaxByteSize\","
		"\"-e\":\"MemoryEstimatePeriod\","
		"\"-g\":\"GracePeriod\","
		"\"-c\":\"SplitConfidence\","
		"\"-t\":\"tieThreshold\","
		"\"-b\":\"binarySplits\","
		"\"-z\":\"stopMemManagement\","
		"\"-r\":\"removePoorAtts\","
		"\"-l\":\"LeafLearner\","
		"\"-q\":\"nbThreshold\","
		"\"-stp\":\"ShowTreePath\","
		"\"-tpil\":\"TreePropertyIndexList\","
		"\"-p\":\"noPrePrune\""
		"}}"
		"");

HoeffdingAdaptiveTree::HoeffdingAdaptiveTree() {
	this->alternateTrees = 0;
	this->prunedAlternateTrees = 0;
	this->switchedAlternateTrees = 0;
}

HoeffdingAdaptiveTree::~HoeffdingAdaptiveTree() {
}

LearningNode* HoeffdingAdaptiveTree::newLearningNode(
		const vector<double>& initialClassObservations) {
	return new AdaLearningNode(initialClassObservations);
}

SplitNode* HoeffdingAdaptiveTree::newSplitNode(
		InstanceConditionalTest* splitTest,
		const vector<double>& classObservations) {
	return new AdaSplitNode(splitTest, classObservations);
}

SplitNode* HoeffdingAdaptiveTree::newSplitNode(
		InstanceConditionalTest* splitTest,
		const vector<double>& classObservations, int size) {
	return new AdaSplitNode(splitTest, classObservations, size);
}

// TODO define a instance count variant
bool debugFlag = false;
int instance_count2 = 0;
void HoeffdingAdaptiveTree::trainOnInstanceImpl(const Instance* inst) {
	instance_count2++;
//	if (instance_count2 > 100000) {
//		exit(0);
//	}
//	debugFlag = (instance_count2 == 11837);
//	cout << instance_count2 << endl;

	if (this->treeRoot == nullptr) {
		this->treeRoot = newLearningNode(vector<double>(0));
		this->activeLeafNodeCount = 1;
	}
	toNewNode(this->treeRoot)->learnFromInstance(inst, this, nullptr, -1);
}

list<FoundNode*>* HoeffdingAdaptiveTree::filterInstanceToLeaves(
		const Instance* inst, SplitNode* parent, int parentBranch,
		bool updateSplitterCounts) {
	list<FoundNode*>* nodes = new list<FoundNode*>;
	toNewNode(this->treeRoot)->filterInstanceToLeaves(inst, parent,
			parentBranch, nodes, updateSplitterCounts);
	return nodes;
}

vector<double>* HoeffdingAdaptiveTree::getVotesForInstance(
		const Instance* inst) {
	this->voteArray.resize(0);
	if (this->treeRoot != nullptr) {
		list<FoundNode*>* foundNodes = filterInstanceToLeaves(inst, nullptr, -1,
				false);
		for (FoundNode* foundNode : *foundNodes) {
			if (foundNode->parentBranch != -999) {
				Node* leafNode = foundNode->node;
				if (leafNode == nullptr) {
					leafNode = foundNode->parent;
				}
				vector<double>* dist = leafNode->getClassVotes(inst, this);

				if (dist->size() > this->voteArray.size()) {
					this->voteArray.resize(dist->size(), 0);
				}
				for (unsigned int i = 0; i < dist->size(); i++) {
					this->voteArray[i] += (*dist)[i];
				}
			}
		}

		// debug: show votes
//		cout << "votes all: " << Utils::maxIndex(this->voteArray);
//		cout << ", [";
//		for (auto v : this->voteArray) {
//			cout << v << " ";
//		}
//		cout << "]";
//		cout << endl;

		return &this->voteArray;
	}
	return &this->voteArray;
}


void HoeffdingAdaptiveTree::toJson(Json::Value& jv) {
	HoeffdingTree::toJson(jv);
	jv["alternateTrees"] = this->alternateTrees;
	jv["prunedAlternateTrees"] = this->prunedAlternateTrees;
	jv["switchedAlternateTrees"] = this->switchedAlternateTrees;
}

bool HoeffdingAdaptiveTree::importFromJson(const Json::Value& jv) {
	HoeffdingTree::importFromJson(jv);

	this->alternateTrees = jv["alternateTrees"].asInt();
	this->prunedAlternateTrees = jv["prunedAlternateTrees"].asInt();
	this->switchedAlternateTrees = jv["switchedAlternateTrees"].asInt();

	return true;
}

bool HoeffdingAdaptiveTree::exportToJson(Json::Value& jv) {
	this->toJson(jv);
	return true;
}


} /* namespace HT */

