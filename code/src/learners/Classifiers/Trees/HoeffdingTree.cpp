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

#include "HoeffdingTree.h"
#include "HATNode.h"

namespace HT {

#include "HTAttributeClassObserver.h"

REGISTER_CLASS(HoeffdingTree)
REGISTER_COMMAND_LINE_PARAMETER(HoeffdingTree, "{\"type\":\"Learner\","
		"\"name\":\"HoeffdingTree\","
		"\"parameter\":{"
		"\"-m\":\"MaxByteSize\","
		"\"-e\":\"MemoryEstimatePeriod\","
		"\"-g\":\"GracePeriod\","
		"\"-c\":\"SplitConfidence\","
		"\"-t\":\"TieThreshold\","
		"\"-b\":\"BinarySplits\","
		"\"-z\":\"StopMemManagement\","
		"\"-r\":\"RemovePoorAtts\","
		"\"-l\":\"LeafLearner\","
		"\"-q\":\"BbThreshold\","
		"\"-stp\":\"ShowTreePath\","
		"\"-tpil\":\"TreePropertyIndexList\","
		"\"-p\":\"NoPrePrune\""
		"}}"
		"");

HoeffdingTree::HoeffdingTree() {
	classPrediction = nullptr;
	trainingWeightSeenByModel = 0.0;
	this->mShowTreePath = false;
	this->mFixTree = false;
	renew();
}

HoeffdingTree::~HoeffdingTree() {
	if (classPrediction != nullptr) {
		delete[] classPrediction;
	}
}

void HoeffdingTree::doSetParams() {
	this->params.maxByteSize = getParam("MaxByteSize",
			this->params.maxByteSize);
	this->params.memoryEstimatePeriod = getParam("MemoryEstimatePeriod",
			this->params.memoryEstimatePeriod);
	this->params.gracePeriod = getParam("GracePeriod",
			this->params.gracePeriod);
	this->params.splitConfidence = getParam("SplitConfidence",
			this->params.splitConfidence);
	this->params.tieThreshold = getParam("TieThreshold",
			this->params.tieThreshold);
	this->params.binarySplits = getParam("BinarySplits",
			this->params.binarySplits);
	this->params.stopMemManagement = getParam("StopMemManagement",
			this->params.stopMemManagement);
	string leafLearner = getParam("LeafLearner", "MC");
	if (leafLearner == "MC") {
		this->params.leafPrediction = 0;
	} else if (leafLearner == "NB") {
		this->params.leafPrediction = 1;
	} else if (leafLearner == "NBAdaptive") {
		this->params.leafPrediction = 2;
	} else {
//		this->params.leafPrediction = 0;
	}

	this->mShowTreePath = (this->getParam("ShowTreePath", 0) == 1);
	string indexList = this->getParam("TreePropertyIndexList", "");
	if (indexList != "") {
		this->mFixTree = true;
		stringstream ss(indexList);
		string value;
		while (getline(ss, value, ',')) {
			this->mTreePropertyIndexList.push_back(atoi(value.c_str()));
		}
	}
}

HoeffdingTree::Params::Params() {
	maxByteSize = 33554432;
	memoryEstimatePeriod = 1000000;
	gracePeriod = 200;
	splitConfidence = 0.0000001f;
	tieThreshold = 0.05;
	binarySplits = false;
	stopMemManagement = false;
//	removePoorAtts;
	noPrePrune = false;
	this->leafPrediction = 0;
	this->nbThreshold = 0;
}

void HoeffdingTree::Params::fromJson(const Json::Value& jv) {
	this->maxByteSize = jv["maxByteSize"].asInt();
	this->memoryEstimatePeriod = jv["memoryEstimatePeriod"].asInt();
	this->gracePeriod = jv["gracePeriod"].asInt();
	this->splitConfidence = jv["splitConfidence"].asDouble();
	this->tieThreshold = jv["tieThreshold"].asDouble();
	this->binarySplits = jv["binarySplits"].asBool();
	this->stopMemManagement = jv["stopMemManagement"].asBool();
	this->numericEstimator = jv["numericEstimator"].asString();
	this->nominalEstimator = jv["nominalEstimator"].asString();
	this->splitCriterion = jv["splitCriterion"].asString();
	this->noPrePrune = jv["noPrePrune"].asBool();
	this->leafPrediction = jv["leafPrediction"].asInt();
	this->nbThreshold = jv["nbThreshold"].asInt();
}

void HoeffdingTree::Params::toJson(Json::Value& jv) {
	jv["maxByteSize"] = this->maxByteSize;
	jv["memoryEstimatePeriod"] = this->memoryEstimatePeriod;
	jv["gracePeriod"] = this->gracePeriod;
	jv["splitConfidence"] = this->splitConfidence;
	jv["tieThreshold"] = this->tieThreshold;
	jv["binarySplits"] = this->binarySplits;
	jv["stopMemManagement"] = this->stopMemManagement;
	jv["numericEstimator"] = this->numericEstimator;
	jv["nominalEstimator"] = this->nominalEstimator;
	jv["splitCriterion"] = this->splitCriterion;
	jv["noPrePrune"] = this->noPrePrune;
	jv["leafPrediction"] = this->leafPrediction;
	jv["nbThreshold"] = this->nbThreshold;
}

void HoeffdingTree::train(const Instance& instance) {
	trainOnInstanceImpl(&instance);
}

double* HoeffdingTree::getPrediction(const Instance& instance) {
	if (classPrediction == nullptr) {
		classPrediction = new double[instance.getNumberClasses()];
	}

	for (int i = 0; i < instance.getNumberClasses(); i++) {
		classPrediction[i] = 0;
	}

	vector<double>* v = getVotesForInstance(&instance);
	for (unsigned int i = 0; i < v->size(); i++) {
		classPrediction[i] = (*v)[i];
	}

	if (this->mShowTreePath) {
		static int rowNumber = 0;
		rowNumber++;
		int predictResult = 0;
		for (unsigned int i = 0; i < v->size(); i++) {
			if ((*v)[i] > (*v)[predictResult]) {
				predictResult = i;
			}
		}
		this->mTreePath.str("");
		if (instance.getLabel() == predictResult) {
			this->mTreePath << rowNumber << " s " << predictResult << " "
					<< predictResult << " ";
		} else {
			this->mTreePath << rowNumber << " f " << instance.getLabel()
					<< " " << predictResult << " ";
		}
		this->showTreePath(instance, this->treeRoot);
		LOG_INFO("path: %s", this->mTreePath.str().c_str());
	}

	return classPrediction;
}

void HoeffdingTree::showTreePath(const Instance& instance, Node* node) {

	if (node != nullptr) {
		if (node->isClass(NT_SplitNode) || node->isClass(NT_AdaSplitNode)) {

			SplitNode* splitNode = (SplitNode*) node;

			InstanceConditionalTest* splitTest = splitNode->splitTest;
			this->mTreePath << " > " << splitTest->getAttIndex() << ","
					<< splitTest->getAttValue() << ","
					<< instance.getInputAttributeValue(
							splitTest->getAttIndex());

			int childIndex = splitTest->branchForInstance(&instance);

			if (childIndex >= 0) {
				node = splitNode->getChild(childIndex);
				if (node != nullptr) {
					showTreePath(instance, node);
				}
			}
		}
	}
	return;
}

double HoeffdingTree::probability(const Instance&, int int1) {
	return 0;
}

void HoeffdingTree::renew() {
	this->treeRoot = nullptr;
	this->decisionNodeCount = 0;
	this->activeLeafNodeCount = 0;
	this->inactiveLeafNodeCount = 0;
	this->inactiveLeafByteSizeEstimate = 0.0;
	this->activeLeafByteSizeEstimate = 0.0;
	this->byteSizeEstimateOverheadFraction = 1.0;
	this->growthAllowed = true;
//    if (this->params.leafprediction.getChosenIndex()>0) {
//        this->params.removePoorAtts = nullptr;
//    }
}

// debug define a instance count variant
int instance_count = 0;

void HoeffdingTree::trainOnInstanceImpl(const Instance* inst) {
	if (this->treeRoot == nullptr) {
		this->treeRoot = newLearningNode();
		this->activeLeafNodeCount = 1;
	}
	FoundNode* foundNode = this->treeRoot->filterInstanceToLeaf(inst, nullptr,
			-1);
	Node* leafNode = foundNode->node;

	// debug: print instance count
	instance_count++;
//	if (instance_count > 3000) {
//		exit(0);
//	}
//	cout << instance_count << endl;

	if (leafNode == nullptr) {
		leafNode = newLearningNode();
		foundNode->parent->setChild(foundNode->parentBranch, leafNode);
		this->activeLeafNodeCount++;
	}

	if (leafNode->isClass(NT_LearningNode)) {
		LearningNode* learningNode = (LearningNode*) leafNode;
		learningNode->learnFromInstance(inst, this);
		if (this->growthAllowed
				&& (learningNode->isClass(NT_ActiveLearningNode))) {
			ActiveLearningNode* activeLearningNode =
					(ActiveLearningNode*) learningNode;

			// debug: showActiveLearningNode
//			if (instance_count == 317483 || instance_count == 321453 || instance_count == 321647 || instance_count == 323017 ) {
//				activeLearningNode->showActiveLearningNode();
//
//				SplitCriterion* splitCriterion = (SplitCriterion*)  new InfoGainSplitCriterion();
//				list<AttributeSplitSuggestion*>* bestSplitSuggestions = activeLearningNode
//						->getBestSplitSuggestions(splitCriterion, this);
//				this->showBestSplitSuggestions(bestSplitSuggestions);
//			}

			double weightSeen = activeLearningNode->getWeightSeen();
			if (weightSeen
					- activeLearningNode->getWeightSeenAtLastSplitEvaluation()
					>= this->params.gracePeriod) {

				// debug: print split id
//				cout << "Try to split, id=" << instance_count << endl;
//				if  (instance_count == 321647 || instance_count == 323017) {
//					activeLearningNode->showActiveLearningNode();
//				}

				attemptToSplit(activeLearningNode, foundNode->parent,
						foundNode->parentBranch);

				// TODO if delete activeLearning in attempyToSplit and set activeLearningNode to nullptr,
				// but activeLearningNode is not nullptr at the following code, why ?
				if (activeLearningNode != nullptr) {
					activeLearningNode->setWeightSeenAtLastSplitEvaluation(
							weightSeen);
				}
			}
		}
	}

	delete foundNode;

//	if (this->trainingWeightSeenByModel % this->params.memoryEstimatePeriod
//			== 0) {
//		estimateModelByteSizes();
//	}
}

vector<double>* HoeffdingTree::getVotesForInstance(const Instance* inst) {
	if (this->treeRoot != nullptr) {
		FoundNode* foundNode = this->treeRoot->filterInstanceToLeaf(inst,
				nullptr, -1);
		Node* leafNode = foundNode->node;
		if (leafNode == nullptr) {
			leafNode = foundNode->parent;
		}
		delete foundNode;
		return leafNode->getClassVotes(inst, this);
	}
	return new vector<double> ;
}

int HoeffdingTree::measureTreeDepth() {
	if (this->treeRoot != nullptr) {
		return this->treeRoot->subtreeDepth();
	}
	return 0;
}

double HoeffdingTree::computeHoeffdingBound(double range, double confidence,
		double n) {
	return sqrt(((range * range) * log(1.0 / confidence)) / (2.0 * n));
}

SplitNode* HoeffdingTree::newSplitNode(InstanceConditionalTest* splitTest,
		const vector<double>& classObservations, int size) {
	return new SplitNode(splitTest, classObservations, size);
}

SplitNode* HoeffdingTree::newSplitNode(InstanceConditionalTest* splitTest,
		const vector<double>& classObservations) {
	return new SplitNode(splitTest, classObservations);
}

AttributeClassObserver* HoeffdingTree::newNominalClassObserver() {
	return new NominalAttributeClassObserver();
}

AttributeClassObserver* HoeffdingTree::newNumericClassObserver() {
	return new GaussianNumericAttributeClassObserver();
}

bool compare_suggestion(AttributeSplitSuggestion* v1,
		AttributeSplitSuggestion* v2) {
	return v1->merit < v2->merit;
}

//bool compare_suggestion_83(AttributeSplitSuggestion* v1,
//		AttributeSplitSuggestion* v2) {
//	return v1->splitTest->attIndex != 83;
//}
//
//bool compare_suggestion_96(AttributeSplitSuggestion* v1,
//		AttributeSplitSuggestion* v2) {
//	return v1->splitTest->attIndex != 96;
//}

void HoeffdingTree::attemptToSplit(ActiveLearningNode* node, SplitNode* parent,
		int parentIndex) {
	if (node->isPure()) {
		return;
	}

	SplitCriterion* splitCriterion = new InfoGainSplitCriterion();
	list<AttributeSplitSuggestion*>* bestSplitSuggestions =
			node->getBestSplitSuggestions(splitCriterion, this);
	int suggestionSize = bestSplitSuggestions->size();
	bestSplitSuggestions->sort(compare_suggestion);
	bool shouldSplit = false;

	if (this->mFixTree
			&& (node->mLevel < this->mTreePropertyIndexList.size())) {
		suggestionSize = 1;
		auto iter = (*bestSplitSuggestions).begin();
		for (; iter != (*bestSplitSuggestions).end(); iter++) {
			if ((*iter)->splitTest != nullptr
					&& (*iter)->splitTest->getAttIndex()
							== this->mTreePropertyIndexList[node->mLevel]) {
				break;
			}
		}
		AttributeSplitSuggestion* ass = (*iter);
		(*bestSplitSuggestions).erase(iter);
		(*bestSplitSuggestions).push_back(ass);
	}

//	if (node->mLevel == 0) {
//		suggestionSize = 1;
//		bestSplitSuggestions->sort(compare_suggestion_83);
//	}else if (node->mLevel == 1) {
//		suggestionSize = 1;
//		bestSplitSuggestions->sort(compare_suggestion_96);
//	}

	if (suggestionSize < 2) {
		shouldSplit = suggestionSize > 0;
	} else {
		double hoeffdingBound = computeHoeffdingBound(
				splitCriterion->getRangeOfMerit(
						*node->getObservedClassDistribution()),
				this->params.splitConfidence, node->getWeightSeen());

		// debug: show hoeffdingBound
//			cout << "hoeffdingBound: " << hoeffdingBound << endl;

		auto iter = bestSplitSuggestions->end();
		iter--;
		AttributeSplitSuggestion* bestSuggestion = (*iter);
		iter--;
		AttributeSplitSuggestion* secondBestSuggestion = (*iter);
		if ((bestSuggestion->merit - secondBestSuggestion->merit
				> hoeffdingBound)
				|| (hoeffdingBound < this->params.tieThreshold)) {
			shouldSplit = true;
		}
	}

	if (shouldSplit) {
		auto iter = bestSplitSuggestions->end();
		iter--;
		AttributeSplitSuggestion* splitDecision = (*iter);

		// debug: show best split suggestion
//			cout << showSplitSuggestion(splitDecision) << endl;

		if (splitDecision->splitTest == nullptr) {
			// preprune - null wins
			deactivateLearningNode(node, parent, parentIndex);
		} else {
			SplitNode* newSplit = newSplitNode(splitDecision->splitTest,
					*node->getObservedClassDistribution(),
					splitDecision->numSplits());
			for (int i = 0; i < splitDecision->numSplits(); i++) {

				// debug: show class distribution array size.
//					cout << (*splitDecision->resultingClassDistributionFromSplit(i)).size() << endl;

				Node* newChild = newLearningNode(
						*splitDecision->resultingClassDistributionFromSplit(i));
				newSplit->setChild(i, newChild);
			}
			this->activeLeafNodeCount--;
			this->decisionNodeCount++;
			this->activeLeafNodeCount += splitDecision->numSplits();
			if (parent == nullptr) {
				this->treeRoot = newSplit;
			} else {
				parent->setChild(parentIndex, newSplit);
				newSplit->mLevel = parent->mLevel + 1;
			}

			for (int i = 0; i < splitDecision->numSplits(); i++) {
				Node* child = newSplit->getChild(i);
				child->mLevel = newSplit->mLevel + 1;
			}

			// TODO why?
//				delete node;
//				node = nullptr;
		}

		// manage memory
		enforceTrackerLimit();
	}

	delete splitCriterion;
	for (auto v : *bestSplitSuggestions) {
		if (v != nullptr) {
			delete v;
		}
	}
	delete bestSplitSuggestions;
}

string HoeffdingTree::showSplitSuggestion(AttributeSplitSuggestion* ass) {
	stringstream sb;
	sb << ass->merit;
	sb << " [";
	if (ass->resultingClassDistributions == nullptr) {
		sb << "null";
	} else {
		for (auto v1 : *ass->resultingClassDistributions) {
			for (double v2 : v1) {
				sb << v2;
				sb << " ";
			}
			sb << " | ";
		}
	}
	sb << "] [";

	if (ass->splitTest == nullptr) {
		sb << "null";
	} else {

		if (ass->splitTest->isClass(T_NumericAttributeBinaryTest)) {
			NumericAttributeBinaryTest* nab =
					(NumericAttributeBinaryTest*) ass->splitTest;
			sb << nab->attIndex;
			sb << " ";
			sb << nab->attValue;
			sb << " ";
			sb << nab->equalsPassesTest;
		} else if (ass->splitTest->isClass(T_NominalAttributeMultiwayTest)) {
			NominalAttributeMultiwayTest* nam =
					(NominalAttributeMultiwayTest*) ass->splitTest;
			sb << nam->attIndex;
		} else if (ass->splitTest->isClass(T_NominalAttributeBinaryTest)) {
			NominalAttributeBinaryTest* nab =
					(NominalAttributeBinaryTest*) ass->splitTest;
			sb << nab->attIndex;
			sb << " ";
			sb << nab->attValue;
		}
	}
	sb << "]";
	return sb.str();
}

void HoeffdingTree::showBestSplitSuggestions(
		list<AttributeSplitSuggestion*>* bestSplitSuggestions) {
	stringstream sb;
	sb << "bss:\n";
	int i = 0;
	for (AttributeSplitSuggestion* ass : *bestSplitSuggestions) {
		sb << i;
		sb << ": ";
		sb << showSplitSuggestion(ass);
		sb << "\n";
		i++;
	}
	cout << sb.str() << endl;
}

bool compare_foundNode(FoundNode* v1, FoundNode* v2) {
	return v1->node->calculatePromise() < v2->node->calculatePromise();
}

void HoeffdingTree::enforceTrackerLimit() {
	if ((this->inactiveLeafNodeCount > 0)
			|| ((this->activeLeafNodeCount * this->activeLeafByteSizeEstimate
					+ this->inactiveLeafNodeCount
							* this->inactiveLeafByteSizeEstimate)
					* this->byteSizeEstimateOverheadFraction
					> this->params.maxByteSize)) {
		if (this->params.stopMemManagement) {
			this->growthAllowed = false;
			return;
		}
		list<FoundNode*>* learningNodes = findLearningNodes();
		learningNodes->sort(compare_foundNode);
		int maxActive = 0;
		int nodeSize = learningNodes->size();
		while (maxActive < nodeSize) {
			maxActive++;
			if ((maxActive * this->activeLeafByteSizeEstimate
					+ (nodeSize - maxActive)
							* this->inactiveLeafByteSizeEstimate)
					* this->byteSizeEstimateOverheadFraction
					> this->params.maxByteSize) {
				maxActive--;
				break;
			}
		}
		int cutoff = nodeSize - maxActive;
		auto iter = learningNodes->begin();
		for (int i = 0; i < cutoff; i++, iter++) {

			if ((*iter)->node->isClass(NT_ActiveLearningNode)) {
				deactivateLearningNode((ActiveLearningNode*) (*iter)->node,
						(*iter)->parent, (*iter)->parentBranch);
			}
		}
		for (int i = cutoff; i < nodeSize; i++, iter++) {
			if ((*iter)->node->isClass(NT_InactiveLearningNode)) {
				activateLearningNode((InactiveLearningNode*) (*iter)->node,
						(*iter)->parent, (*iter)->parentBranch);
			}
		}

		for (auto p : *learningNodes) {
			delete p;
		}
		delete learningNodes;
	}
}

void HoeffdingTree::estimateModelByteSizes() {
//	list<FoundNode>* learningNodes = findLearningNodes();
//	long totalActiveSize = 0;
//	long totalInactiveSize = 0;
//	auto iter = learningNodes->begin();
//	for (int i=0; iter!=learningNodes->end(); iter++,i++) {
//		if (((LearningNode*)(*iter))->activeNodeType == ActiveLearning) {
//			totalActiveSize += SizeOf.fullSizeOf(foundNode.node);
//		} else {
//			totalInactiveSize += SizeOf.fullSizeOf(foundNode.node);
//		}
//	}
//	if (totalActiveSize > 0) {
//		this->activeLeafByteSizeEstimate = (double) totalActiveSize
//				/ this->activeLeafNodeCount;
//	}
//	if (totalInactiveSize > 0) {
//		this->inactiveLeafByteSizeEstimate = (double) totalInactiveSize
//				/ this->inactiveLeafNodeCount;
//	}
//	int actualModelSize = this->measureByteSize();
//	double estimatedModelSize = (this->activeLeafNodeCount
//			* this->activeLeafByteSizeEstimate
//			+ this->inactiveLeafNodeCount * this->inactiveLeafByteSizeEstimate);
//	this->byteSizeEstimateOverheadFraction = actualModelSize
//			/ estimatedModelSize;
//	if (actualModelSize > this->maxByteSizeOption.getValue()) {
//		enforceTrackerLimit();
//	}
}

void HoeffdingTree::deactivateAllLeaves() {
	list<FoundNode*>* learningNodes = findLearningNodes();
	auto iter = learningNodes->begin();
	for (int i = 0; iter != learningNodes->end(); i++, iter++) {
		if ((*iter)->node->isClass(NT_ActiveLearningNode)) {
			deactivateLearningNode((ActiveLearningNode*) (*iter)->node,
					(*iter)->parent, (*iter)->parentBranch);
		}
		delete (*iter);
	}
	delete learningNodes;
}

void HoeffdingTree::deactivateLearningNode(ActiveLearningNode* toDeactivate,
		SplitNode* parent, int parentBranch) {
	Node* newLeaf = new InactiveLearningNode(
			*toDeactivate->getObservedClassDistribution());
	if (parent == nullptr) {
		this->treeRoot = newLeaf;
	} else {
		parent->setChild(parentBranch, newLeaf);
	}
	this->activeLeafNodeCount--;
	this->inactiveLeafNodeCount++;
	delete toDeactivate;
}

void HoeffdingTree::activateLearningNode(InactiveLearningNode* toActivate,
		SplitNode* parent, int parentBranch) {
	Node* newLeaf = newLearningNode(
			*toActivate->getObservedClassDistribution());
	if (parent == nullptr) {
		this->treeRoot = newLeaf;
	} else {
		parent->setChild(parentBranch, newLeaf);
	}
	this->activeLeafNodeCount++;
	this->inactiveLeafNodeCount--;
	delete toActivate;
}

list<FoundNode*>* HoeffdingTree::findLearningNodes() {
	list<FoundNode*>* foundList = new list<FoundNode*>();
	findLearningNodes(this->treeRoot, nullptr, -1, foundList);
	return foundList;

}

void HoeffdingTree::findLearningNodes(Node* node, SplitNode* parent,
		int parentBranch, list<FoundNode*>* found) {
	if (node != nullptr) {
		if (node->isClass(NT_LearningNode)) {
			found->push_back(new FoundNode(node, parent, parentBranch));
		}
		if (node->isClass(NT_SplitNode)) {
			SplitNode* splitNode = (SplitNode*) node;
			auto iter = splitNode->children->begin();
			for (int i = 0; iter != splitNode->children->end(); iter++, i++) {
				findLearningNodes((*iter), splitNode, i, found);
			}
		}
	}
}

LearningNode* HoeffdingTree::newLearningNode() {
	return newLearningNode(vector<double>(0));
}

LearningNode* HoeffdingTree::newLearningNode(
		const vector<double>& initialClassObservations) {
	LearningNode* ret;
	if (this->params.leafPrediction == 0) { //MC
		ret = new ActiveLearningNode(initialClassObservations);
	} else if (this->params.leafPrediction == 1) { //NB
		ret = new LearningNodeNB(initialClassObservations);
	} else { //NBAdaptive
		ret = new LearningNodeNBAdaptive(initialClassObservations);
	}
	return ret;
}

void HoeffdingTree::toJson(Json::Value& jv) {
	this->params.toJson(jv["params"]);

	jv["decisionNodeCount"] = this->decisionNodeCount;
	jv["activeLeafNodeCount"] = this->activeLeafNodeCount;
	jv["inactiveLeafNodeCount"] = this->inactiveLeafNodeCount;
	jv["inactiveLeafByteSizeEstimate"] = this->inactiveLeafByteSizeEstimate;
	jv["activeLeafByteSizeEstimate"] = this->activeLeafByteSizeEstimate;
	jv["byteSizeEstimateOverheadFraction"] =
			this->byteSizeEstimateOverheadFraction;
	jv["growthAllowed"] = this->growthAllowed;
	jv["trainingWeightSeenByModel"] = this->trainingWeightSeenByModel;

	this->treeRoot->toJson(jv["treeRoot"]);
}

bool HoeffdingTree::importFromJson(const Json::Value& jv) {
	this->params.fromJson(jv["params"]);

	this->decisionNodeCount = jv["decisionNodeCount"].asInt();
	this->activeLeafNodeCount = jv["activeLeafNodeCount"].asInt();
	this->inactiveLeafNodeCount = jv["inactiveLeafNodeCount"].asInt();
	this->inactiveLeafByteSizeEstimate =
			jv["inactiveLeafByteSizeEstimate"].asDouble();
	this->activeLeafByteSizeEstimate =
			jv["activeLeafByteSizeEstimate"].asDouble();
	this->byteSizeEstimateOverheadFraction =
			jv["byteSizeEstimateOverheadFraction"].asDouble();
	this->growthAllowed = jv["growthAllowed"].asBool();
	this->trainingWeightSeenByModel =
			jv["trainingWeightSeenByModel"].asDouble();

	if (treeRoot != nullptr) {
		delete treeRoot;
	}

	// new node
	string nodeType = jv["treeRoot"]["type"].asString();
	if (nodeType == "Node") {
		this->treeRoot = new Node(jv["treeRoot"]);
	} else if (nodeType == "SplitNode") {
		this->treeRoot = new SplitNode(jv["treeRoot"]);
	} else if (nodeType == "InactiveLearningNode") {
		this->treeRoot = new InactiveLearningNode(jv["treeRoot"]);
	} else if (nodeType == "ActiveLearningNode") {
		this->treeRoot = new ActiveLearningNode(jv["treeRoot"]);
	} else if (nodeType == "LearningNodeNB") {
		this->treeRoot = new LearningNodeNB(jv["treeRoot"]);
	} else if (nodeType == "LearningNodeNBAdaptive") {
		this->treeRoot = new LearningNodeNBAdaptive(jv["treeRoot"]);
	} else if (nodeType == "AdaSplitNode") {
		this->treeRoot = new AdaSplitNode(jv["treeRoot"]);
	} else if (nodeType == "AdaLearningNode") {
		this->treeRoot = new AdaLearningNode(jv["treeRoot"]);
	}

	return true;
}

bool HoeffdingTree::exportToJson(Json::Value& jv) {
	this->toJson(jv);
	return true;
}

} /* namespace HT */

