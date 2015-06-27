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

#ifndef HOEFFDINGTREE_H_
#define HOEFFDINGTREE_H_

#include <vector>
#include <string>
#include <list>
#include "../../../Common.h"
#include "HTAttributeClassObserver.h"
#include "InstanceConditionalTest.h"
#include "HTNode.h"
using namespace std;

namespace HT {

class HoeffdingTree: public Learner {
public:
	HoeffdingTree();
	virtual ~HoeffdingTree();

	void train(const Instance&);
	double* getPrediction(const Instance&);
	double probability(const Instance&, int);

public:
	double* classPrediction;

	struct Params {
		int maxByteSize;// "Maximum memory consumed by the tree.", 33554432, 0
		int memoryEstimatePeriod;// "How many instances between memory consumption checks.", 1000000, 0
		int gracePeriod;// "The number of instances a leaf should observe between split attempts.", 200, 0,
		float splitConfidence;// "The allowable error in split decision, values closer to 0 will take longer to decide.", 0.0000001, 0.0, 1.0
		float tieThreshold;	// "Threshold below which a split will be forced to break ties.", 0.05, 0.0, 1.0
		bool binarySplits;			// "Only allow binary splits."
		bool stopMemManagement;	// "Stop growing as soon as memory limit is hit."
		string numericEstimator;// "numericEstimator",  'n', "Numeric estimator to use.", NumericAttributeClassObserver.class, "GaussianNumericAttributeClassObserver");
		string nominalEstimator;// "nominalEstimator",  'd', "Nominal estimator to use.", DiscreteAttributeClassObserver.class, "NominalAttributeClassObserver");
		string splitCriterion; // "splitCriterion", 's', "Split criterion to use.", SplitCriterion.class, "InfoGainSplitCriterion");
		//		 removePoorAtts;		// "Disable poor attributes."
//		leafprediction;
		bool noPrePrune;			// "Disable pre-pruning."
		int leafPrediction;		// "LeafLearner" 0: MC, 1: NB, 2: NBAdaptive;
		int nbThreshold;// "The number of instances a leaf should observe before permitting Naive Bayes.", 0, 0, Integer.MAX_VALUE);
		Params();
		void fromJson(const Json::Value& jv);
		void toJson(Json::Value& jv);
	};
	Params params;

	Node* treeRoot;
	int decisionNodeCount;
	int activeLeafNodeCount;
	int inactiveLeafNodeCount;
	double inactiveLeafByteSizeEstimate;
	double activeLeafByteSizeEstimate;
	double byteSizeEstimateOverheadFraction;
	bool growthAllowed;
	double trainingWeightSeenByModel;

	vector<double> voteArray;

	void doSetParams();
	void renew();
	virtual void trainOnInstanceImpl(const Instance* inst);
	virtual vector<double>* getVotesForInstance(const Instance* inst);
	int measureTreeDepth();
	double computeHoeffdingBound(double range, double confidence, double n);
	virtual SplitNode* newSplitNode(InstanceConditionalTest* splitTest,
			const vector<double>& classObservations, int size);
	virtual SplitNode* newSplitNode(InstanceConditionalTest* splitTest,
			const vector<double>& classObservations);
	AttributeClassObserver* newNominalClassObserver();
	AttributeClassObserver* newNumericClassObserver();
	void attemptToSplit(ActiveLearningNode* node, SplitNode* parent,
			int parentIndex);
	void enforceTrackerLimit();
	void estimateModelByteSizes();
	void deactivateAllLeaves();
	void deactivateLearningNode(ActiveLearningNode* toDeactivate,
			SplitNode* parent, int parentBranch);
	void activateLearningNode(InactiveLearningNode* toActivate,
			SplitNode* parent, int parentBranch);
	list<FoundNode*>* findLearningNodes();
	void findLearningNodes(Node* node, SplitNode* parent, int parentBranch,
			list<FoundNode*>* found);

	virtual LearningNode* newLearningNode();
	virtual LearningNode* newLearningNode(
			const vector<double>& initialClassObservations);

	virtual void toJson(Json::Value& jv);

private:
	string showSplitSuggestion(AttributeSplitSuggestion* ass);
	void showBestSplitSuggestions(
			list<AttributeSplitSuggestion*>* bestSplitSuggestions);

	bool mShowTreePath;
	stringstream mTreePath;
	void showTreePath(const Instance& instance,  Node* node);

protected:
	virtual bool importFromJson(const Json::Value& jv);
	virtual bool exportToJson(Json::Value& jv);

	bool mFixTree;
	vector<int> mTreePropertyIndexList;
};
// class HoeffdingTree

} /* namespace HT */

#endif /* HOEFFDINGTREE_H_ */
