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

#include "HATNode.h"
#include "../../../Common.h"
#include "HoeffdingAdaptiveTree.h"

namespace HT {

NewNode::NewNode() {

}

NewNode::~NewNode() {

}

NewNode* toNewNode(Node* node) {
	NewNode* nn = nullptr;
	if (node->isClass(NT_AdaSplitNode)) {
		nn = (AdaSplitNode*) node;
	} else if (node->isClass(NT_AdaLearningNode)) {
		nn = (AdaLearningNode*) node;
	}else {
		cout << "something is wrong" << endl;
	}

	return nn;
}

AdaSplitNode::AdaSplitNode(InstanceConditionalTest* splitTest,
		const vector<double>& classObservations, int size) :
		SplitNode(splitTest, classObservations, size) {
	this->alternateTree = nullptr;
	this->estimationErrorWeight = nullptr;
	this->ErrorChange = false;
	this->mClassTypes = {NT_AdaSplitNode};
}

AdaSplitNode::AdaSplitNode(InstanceConditionalTest* splitTest,
		const vector<double>& classObservations) :
		SplitNode(splitTest, classObservations) {
	this->alternateTree = nullptr;
	this->estimationErrorWeight = nullptr;
	this->ErrorChange = false;
	this->mClassTypes = {NT_AdaSplitNode};
}

AdaSplitNode::AdaSplitNode(const Json::Value& jv):SplitNode(jv) {
	if (jv["alternateTree"].isString()) {
		this->alternateTree = nullptr;
	}else{
		string nodeType = jv["alternateTree"]["type"].asString();
		if (nodeType == "Node") {
			this->alternateTree = new Node(jv["alternateTree"]);
		} else if (nodeType == "SplitNode") {
			this->alternateTree = new SplitNode(jv["alternateTree"]);
		} else if (nodeType == "InactiveLearningNode") {
			this->alternateTree = new InactiveLearningNode(jv["alternateTree"]);
		} else if (nodeType == "ActiveLearningNode") {
			this->alternateTree = new ActiveLearningNode(jv["alternateTree"]);
		} else if (nodeType == "LearningNodeNB") {
			this->alternateTree = new LearningNodeNB(jv["alternateTree"]);
		} else if (nodeType == "LearningNodeNBAdaptive") {
			this->alternateTree = new LearningNodeNBAdaptive(jv["alternateTree"]);
		} else if (nodeType == "AdaSplitNode") {
			this->alternateTree = new AdaSplitNode(jv["alternateTree"]);
		} else if (nodeType == "AdaLearningNode") {
			this->alternateTree = new AdaLearningNode(jv["alternateTree"]);
		}
	}

	this->ErrorChange = jv["ErrorChange"].asBool();
//	this->mClassTypes = {NT_AdaSplitNode, NT_NewNode, NT_SplitNode, NT_Node};
	this->mClassTypes = {NT_AdaSplitNode};

	if (jv["estimationErrorWeight"].isString()) {
		this->estimationErrorWeight = nullptr;
	}else {
		this->estimationErrorWeight = new ADWIN(jv["estimationErrorWeight"]);
	}
}


void  AdaSplitNode::toJson(Json::Value& jv) {
	SplitNode::toJson(jv);

	jv["type"] = "AdaSplitNode";

	if (this->alternateTree == nullptr) {
		jv["alternateTree"] = "nullptr";
	}else {
		this->alternateTree->toJson(jv["alternateTree"]);
	}

	if (this->estimationErrorWeight == nullptr) {
		jv["estimationErrorWeight"] = "nullptr";
	}else{
		this->estimationErrorWeight->toJson(jv["estimationErrorWeight"]);
	}
	jv["ErrorChange"] = this->ErrorChange;
}

int AdaSplitNode::numberLeaves() {
	int numLeaves = 0;
	for (Node* child : *this->children) {
		if (child != nullptr) {
			numLeaves += toNewNode(child)->numberLeaves();
		}
	}
	return numLeaves + 1;
}

double AdaSplitNode::getErrorEstimation() {
	return this->estimationErrorWeight->getEstimation();
}

double AdaSplitNode::getErrorWidth() {
	double w = 0.0;
	if (isNullError() == false) {
		w = this->estimationErrorWeight->getWidth();
	}
	return w;
}

bool AdaSplitNode::isNullError() {
	return (this->estimationErrorWeight == nullptr);
}

void AdaSplitNode::learnFromInstance(const Instance* inst,
		HoeffdingAdaptiveTree* ht, SplitNode* parent, int parentBranch) {
	int trueClass = (int) inst->getLabel();

	//New option vore
//	int k = Utils::poisson(1.0);
//	Instance* weightedInst = cloneInstance(inst);
	const Instance* weightedInst = inst;

	int ClassPrediction = 0;
	FoundNode* tempNodes = filterInstanceToLeaf(inst, parent, parentBranch);
	if (tempNodes->node != nullptr) {
		ClassPrediction = Utils::maxIndex(
				*(tempNodes->node->getClassVotes(inst, ht)));
	}
	delete tempNodes;
	bool blCorrect = (trueClass == ClassPrediction);

	if (this->estimationErrorWeight == nullptr) {
		this->estimationErrorWeight = new ADWIN();
	}

	double oldError = this->getErrorEstimation();
	this->ErrorChange = this->estimationErrorWeight->setInput(
			blCorrect == true ? 0.0 : 1.0);
	if (this->ErrorChange == true && oldError > this->getErrorEstimation()) {
		//if error is decreasing, don't do anything
		this->ErrorChange = false;
	}

	// Check condition to build a new alternate tree
	//if (this->isAlternateTree == false) {
	if (this->ErrorChange == true) {       //&& this->alternateTree == nullptr {

//		cout << "Start a new alternative tree, id=" << instance_count2 << endl;

		// TODO this->alternateTree is nullptr or not?
		//Start a new alternative tree : learning node
		this->alternateTree = ht->newLearningNode(vector<double>(0));
		//this->alternateTree.isAlternateTree = true;
		ht->alternateTrees++;
	} // Check condition to replace tree
	else if (this->alternateTree != nullptr
			&& toNewNode(this->alternateTree)->isNullError() == false) {
		if (this->getErrorWidth() > 300
				&& toNewNode(this->alternateTree)->getErrorWidth() > 300) {
			double oldErrorRate = this->getErrorEstimation();
			double altErrorRate =
					toNewNode(this->alternateTree)->getErrorEstimation();
			double fDelta = 0.05f;
			//if (gNumAlts>0) fDelta=fDelta/gNumAlts;
			double fN = 1.0
					/ ((double) toNewNode(this->alternateTree)->getErrorWidth())
					+ 1.0 / ((double) this->getErrorWidth());
			double Bound = (double) sqrt(
					(double) 2.0 * oldErrorRate * (1.0 - oldErrorRate)
							* log(2.0 / fDelta) * fN);
			if (Bound < oldErrorRate - altErrorRate) {
				// Switch alternate tree

//				cout << "Switch alternate tree, id=" << instance_count2 << endl;

				ht->activeLeafNodeCount -= this->numberLeaves();
				ht->activeLeafNodeCount +=
						toNewNode(this->alternateTree)->numberLeaves();
				killTreeChilds(ht);
				if (parent != nullptr) {
					parent->setChild(parentBranch, this->alternateTree);
				} else {
					// Switch root tree
					ht->treeRoot =
							((AdaSplitNode*) ht->treeRoot)->alternateTree;
				}
				ht->switchedAlternateTrees++;
			} else if (Bound < altErrorRate - oldErrorRate) {
				// Erase alternate tree

//				cout << "Erase alternate tree, id=" << instance_count2 << endl;

				if (this->alternateTree->isClass(NT_ActiveLearningNode)) {
					delete this->alternateTree;
					this->alternateTree = nullptr;
				} else if (this->alternateTree->isClass(
						NT_InactiveLearningNode)) {
					delete this->alternateTree;
					this->alternateTree = nullptr;
				} else {
					((AdaSplitNode*) this->alternateTree)->killTreeChilds(ht);
				}
				ht->prunedAlternateTrees++;
			}
		}
	}

	//}
	//learnFromInstance alternate Tree and Child nodes
	if (this->alternateTree != nullptr) {
		toNewNode(this->alternateTree)->learnFromInstance(weightedInst, ht,
				parent, parentBranch);
	}

	int childBranch = this->instanceChildIndex(inst);
	Node* child = this->getChild(childBranch);
	if (child != nullptr) {
		toNewNode(child)->learnFromInstance(weightedInst, ht, this,
				childBranch);
	}

//	delete weightedInst;
}

void AdaSplitNode::killTreeChilds(HoeffdingAdaptiveTree* ht) {
	auto iter = this->children->begin();
	for (; iter != this->children->end(); iter++) {
		Node* child = (*iter);
		if (child != nullptr) {
			//Delete alternate tree if it exists
			if (child->isClass(NT_AdaSplitNode)
					&& ((AdaSplitNode*) child)->alternateTree != nullptr) {
				toNewNode(((AdaSplitNode*) child)->alternateTree)->killTreeChilds(
						ht);
				ht->prunedAlternateTrees++;
			}
			//Recursive delete of SplitNodes
			if (child->isClass(NT_AdaSplitNode)) {
				((AdaSplitNode*) child)->killTreeChilds(ht);
			}
			if (child->isClass(NT_ActiveLearningNode)) {
				delete child;
				(*iter) = nullptr;
				ht->activeLeafNodeCount--;
			} else if (child->isClass(NT_InactiveLearningNode)) {
				delete child;
				(*iter) = nullptr;
				ht->inactiveLeafNodeCount--;
			}
		}
	}
}

void AdaSplitNode::filterInstanceToLeaves(const Instance* inst,
		SplitNode* myparent, int parentBranch, list<FoundNode*>*& foundNodes,
		bool updateSplitterCounts) {
	if (updateSplitterCounts) {
		Utils::addToValue(*this->observedClassDistribution, inst->getLabel(),
				inst->getWeight());
	}
	int childIndex = instanceChildIndex(inst);
	if (childIndex >= 0) {
		Node* child = getChild(childIndex);
		if (child != nullptr) {
			toNewNode(child)->filterInstanceToLeaves(inst, this, childIndex,
					foundNodes, updateSplitterCounts);
		} else {
			foundNodes->push_back(new FoundNode(nullptr, this, childIndex));
		}
	}
	if (this->alternateTree != nullptr) {
		toNewNode(this->alternateTree)->filterInstanceToLeaves(inst, this, -999,
				foundNodes, updateSplitterCounts);
	}
}



AdaLearningNode::AdaLearningNode(const vector<double>& initialClassObservations) :
		LearningNodeNBAdaptive(initialClassObservations) {
//	this->mClassTypes = {NT_AdaLearningNode, NT_NewNode, NT_LearningNodeNBAdaptive, NT_LearningNodeNB, NT_ActiveLearningNode, NT_LearningNode, NT_Node};
	this->mClassTypes = {NT_AdaLearningNode};
	this->estimationErrorWeight = nullptr;
	this->ErrorChange = false;
}

AdaLearningNode::AdaLearningNode(const Json::Value& jv):LearningNodeNBAdaptive(jv) {
	this->ErrorChange = jv["ErrorChange"].asBool();
//	this->mClassTypes = {NT_AdaLearningNode, NT_NewNode, NT_LearningNodeNBAdaptive, NT_LearningNodeNB, NT_ActiveLearningNode, NT_LearningNode, NT_Node};
	this->mClassTypes = {NT_AdaLearningNode};

	if (jv["estimationErrorWeight"].isString()) {
		this->estimationErrorWeight = nullptr;
	}else {
		this->estimationErrorWeight = new ADWIN(jv["estimationErrorWeight"]);
	}
}


void AdaLearningNode::toJson(Json::Value& jv) {
	LearningNodeNBAdaptive::toJson(jv);

	jv["type"] = "AdaLearningNode";
	jv["ErrorChange"] = this->ErrorChange;

	if (this->estimationErrorWeight == nullptr) {
		jv["estimationErrorWeight"] = "nullptr";
	}else{
		this->estimationErrorWeight->toJson(jv["estimationErrorWeight"]);
	}
}

int AdaLearningNode::numberLeaves() {
	return 1;
}

double AdaLearningNode::getErrorEstimation() {
	if (this->estimationErrorWeight != nullptr) {
		return this->estimationErrorWeight->getEstimation();
	} else {
		return 0;
	}
}

double AdaLearningNode::getErrorWidth() {
	return this->estimationErrorWeight->getWidth();
}

bool AdaLearningNode::isNullError() {
	return (this->estimationErrorWeight == nullptr);
}

void AdaLearningNode::killTreeChilds(HoeffdingAdaptiveTree* ht) {
}

void AdaLearningNode::learnFromInstance(const Instance* inst,
		HoeffdingAdaptiveTree* ht, SplitNode* parent, int parentBranch) {
	int trueClass = (int) inst->getLabel();

	//New option vore
	int k = Utils::poisson(1.0);
	Instance* weightedInst = cloneInstance(inst);
	// TODO cancel random
	if (k > 0) {
		weightedInst->setWeight(inst->getWeight() * k);
	}

	//Compute ClassPrediction using filterInstanceToLeaf
	int ClassPrediction = Utils::maxIndex(*(this->getClassVotes(inst, ht)));
	bool blCorrect = (trueClass == ClassPrediction);

	if (this->estimationErrorWeight == nullptr) {
		this->estimationErrorWeight = new ADWIN();
	}
	double oldError = this->getErrorEstimation();
	this->ErrorChange = this->estimationErrorWeight->setInput(
			blCorrect == true ? 0.0 : 1.0);
	if (this->ErrorChange == true && oldError > this->getErrorEstimation()) {
		this->ErrorChange = false;
	}

	//Update statistics
	LearningNodeNBAdaptive::learnFromInstance(weightedInst, ht);	//inst
	delete weightedInst;

	//Check for Split condition
	double weightSeen = this->getWeightSeen();
	if (weightSeen - this->getWeightSeenAtLastSplitEvaluation()
			>= ht->params.gracePeriod) {
		ht->attemptToSplit(this, parent, parentBranch);
		this->setWeightSeenAtLastSplitEvaluation(weightSeen);
	}

	//learnFromInstance alternate Tree and Child nodes
	/*if (this->alternateTree != nullptr  {
	 this->alternateTree.learnFromInstance(inst,ht);
	 }
	 for (Node child : this->children) {
	 if (child != nullptr {
	 child.learnFromInstance(inst,ht);
	 }
	 }*/
}

vector<double>* AdaLearningNode::getClassVotes(const Instance* inst,
		HoeffdingTree* ht) {
	vector<double>* dist = &this->classVotes;
	int predictionOption = ((HoeffdingAdaptiveTree*) ht)->params.leafPrediction;
	if (predictionOption == 0) { //MC
		(*dist) = (*this->observedClassDistribution);
	} else if (predictionOption == 1) { //NB
		(*dist) = *doNaiveBayesPrediction(inst,
				*this->observedClassDistribution, *this->attributeObservers);
	} else { //NBAdaptive
		if (this->mcCorrectWeight > this->nbCorrectWeight) {
			*dist = *this->observedClassDistribution;
		} else {
			*dist = *doNaiveBayesPrediction(inst,
					*this->observedClassDistribution,
					*this->attributeObservers);
		}
	}

	//New for option votes
	double distSum = Utils::sum(*dist);
	if (distSum * this->getErrorEstimation() * this->getErrorEstimation()
			> 0.0) {
		Utils::normalize(*dist,
				distSum * this->getErrorEstimation()
						* this->getErrorEstimation()); //Adding weight
	}

	return dist;
}

void AdaLearningNode::filterInstanceToLeaves(const Instance* inst,
		SplitNode* splitparent, int parentBranch, list<FoundNode*>*& foundNodes,
		bool updateSplitterCounts) {
	foundNodes->push_back(new FoundNode(this, splitparent, parentBranch));
}

} /* namespace HT */

