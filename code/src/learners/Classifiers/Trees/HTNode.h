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

#ifndef HTNODE_H_
#define HTNODE_H_

#include <vector>
#include <string>
#include <list>
#include "../../../Common.h"
#include "HTAttributeClassObserver.h"
#include "InstanceConditionalTest.h"
#include "AttributeSplitSuggestion.h"
#include "HTNode.h"
using namespace std;

namespace HT {

class Node;
class SplitNode;
class HoeffdingTree;

class FoundNode {
public:
	Node* node;
	SplitNode* parent;
	int parentBranch;
	FoundNode(Node* node, SplitNode* parent, int parentBranch);
	void toJson(Json::Value& jv);
};

enum NodeType {
	NT_Node,
	NT_SplitNode,
	NT_LearningNode,
	NT_InactiveLearningNode,
	NT_ActiveLearningNode,
	NT_LearningNodeNB,
	NT_LearningNodeNBAdaptive,
	NT_NewNode,
	NT_AdaSplitNode,
	NT_AdaLearningNode
};

class Node : public RTTI{
public:
	vector<double>* observedClassDistribution;

	Node(const vector<double>& classObservations);
	Node(const Json::Value& jv);
	virtual ~Node();
	virtual bool isLeaf();
	virtual FoundNode* filterInstanceToLeaf(const Instance* inst, SplitNode* parent,
			int parentBranch);
	virtual vector<double>* getObservedClassDistribution() ;
	virtual vector<double>* getClassVotes(const Instance* inst, HoeffdingTree* ht) ;
	virtual bool isPure();
	virtual void toJson(Json::Value& jv);
	virtual int subtreeDepth();
	virtual double calculatePromise();

	int mLevel;

//	void addToValue(int index, double value);
//	double sumOfValues();
};


class SplitNode: public Node {
public:
	InstanceConditionalTest* splitTest;
	list<Node*>* children;
	int childrenSize;

	SplitNode(InstanceConditionalTest* splitTest, const vector<double>& classObservations, int size);
	SplitNode(InstanceConditionalTest* splitTest, const vector<double>& classObservations);
	SplitNode(const Json::Value& jv);
	~SplitNode();
	virtual int numChildren();
	virtual void setChild(int index, Node* child);
	virtual Node* getChild(int index);
	virtual int instanceChildIndex(const Instance* inst);
	virtual bool isLeaf();
	virtual FoundNode* filterInstanceToLeaf(const Instance* inst, SplitNode* parent,
			int parentBranch);
	virtual void toJson(Json::Value& jv);
	virtual int subtreeDepth();

};

class LearningNode: public Node {
public:
	LearningNode(const vector<double>& initialClassObservations);
	LearningNode(const Json::Value& jv);
	virtual void learnFromInstance(const Instance* inst, HoeffdingTree* ht) = 0;
};


class InactiveLearningNode: public LearningNode {
public:
	InactiveLearningNode(const vector<double>& initialClassObservations);
	InactiveLearningNode(const Json::Value& jv);
	virtual void learnFromInstance(const Instance* inst, HoeffdingTree* ht);
	virtual void toJson(Json::Value& jv);
};


class ActiveLearningNode: public LearningNode {
public:
	double weightSeenAtLastSplitEvaluation;
	list<AttributeClassObserver*>* attributeObservers;
	int attrObsSize;
	bool isInitialized;
	ActiveLearningNode(const vector<double>& initialClassObservations);
	ActiveLearningNode(const Json::Value& jv);
	~ActiveLearningNode();
	virtual void learnFromInstance(const Instance* inst, HoeffdingTree* ht);
	virtual double getWeightSeen() const;
	virtual double getWeightSeenAtLastSplitEvaluation();
	virtual void setWeightSeenAtLastSplitEvaluation(double weight);
	virtual list<AttributeSplitSuggestion*>* getBestSplitSuggestions(SplitCriterion* criterion, HoeffdingTree* ht);
	virtual void disableAttribute(int attIndex);
	virtual void toJson(Json::Value& jv);
	void showActiveLearningNode();
};


class LearningNodeNB: public ActiveLearningNode {
public:
	vector<double> classVotes;	// used for save votes, don't need to clean memory after calling getClassVotes()

	LearningNodeNB(const vector<double>& initialClassObservations);
	LearningNodeNB(const Json::Value& jv);
	~LearningNodeNB();
	virtual vector<double>* getClassVotes(const Instance* inst, HoeffdingTree* ht) ;
	virtual void disableAttribute(int attIndex);
	virtual void toJson(Json::Value& jv);
	vector<double>* doNaiveBayesPrediction(const Instance* inst,
	            const vector<double>& observedClassDistribution,
	            const list<AttributeClassObserver*>& attributeObservers)  ;
};

class LearningNodeNBAdaptive: public LearningNodeNB {
public:
	double mcCorrectWeight;
	double nbCorrectWeight;
	LearningNodeNBAdaptive(const vector<double>& initialClassObservations);
	LearningNodeNBAdaptive(const Json::Value& jv);
	virtual void learnFromInstance(const Instance* inst, HoeffdingTree* ht);
	virtual vector<double>* getClassVotes(const Instance* inst, HoeffdingTree* ht) ;
	virtual void toJson(Json::Value& jv);
};

} /* namespace HT */

#endif /* HTNODE_H_ */
