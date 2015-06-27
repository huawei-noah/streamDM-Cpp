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

#include "HTNode.h"
#include "HATNode.h"
#include "HoeffdingTree.h"

namespace HT {

int modelAttIndexToInstanceAttIndex(int index, const Instance* inst) {
	return inst->getLabel() > index ? index : index + 1;
}

FoundNode::FoundNode(Node* node, SplitNode* parent, int parentBranch) {
	this->node = node;
	this->parent = parent;
	this->parentBranch = parentBranch;
}

void FoundNode::toJson(Json::Value& jv) {
	if (this->node == nullptr) {
		jv["node"] = "nullptr";
	} else {
		this->node->toJson(jv["node"]);
	}
	if (this->parent == nullptr) {
		jv["parent"] = "nullptr";
	} else {
		this->parent->toJson(jv["parent"]);
	}
	jv["parentBranch"] = this->parentBranch;
}

Node::Node(const vector<double>& classObservations) {
	observedClassDistribution = new vector<double>(classObservations.size());
	for (unsigned int i = 0; i < observedClassDistribution->size(); i++) {
		(*observedClassDistribution)[i] = classObservations[i];
	}

	this->mClassTypes = {NT_Node};
	this->mLevel = 0;	// debug
}

Node::Node(const Json::Value& jv) {
	if (!jv["observedClassDistribution"].isNull()) {
		int size = jv["observedClassDistribution"].size();
		this->observedClassDistribution = new vector<double>(size);
		for (int i = 0; i < size; i++) {
			(*(this->observedClassDistribution))[i] =
					jv["observedClassDistribution"][i].asDouble();
		}
	}
	this->mClassTypes = {NT_Node};
	this->mLevel = 0;	// debug
}

Node::~Node() {
	delete observedClassDistribution;
}

FoundNode* Node::filterInstanceToLeaf(const Instance* inst, SplitNode* parent,
		int parentBranch) {
	return new FoundNode(this, parent, parentBranch);
}

bool Node::isLeaf() {
	return true;
}

vector<double>* Node::getObservedClassDistribution() {
	return observedClassDistribution;
}

vector<double>* Node::getClassVotes(const Instance* inst, HoeffdingTree* ht) {
	return observedClassDistribution;
}

bool Node::isPure() {
	const vector<double> &vec = (*observedClassDistribution);
	int count = 0;
	for (unsigned int i = 0; i < vec.size(); i++) {
		if (vec[i] != 0.0) {
			count++;
		}
	}
	return count < 2;
}

void Node::toJson(Json::Value& jv) {
	jv["type"] = "Node";
	if (observedClassDistribution == nullptr) {
		jv["observedClassDistribution"] = "nullptr";
	} else {
		int i = 0;
		for (double d : *observedClassDistribution) {
			jv["observedClassDistribution"][i] = d;
			i++;
		}
	}
}

int Node::subtreeDepth() {
	return 0;
}

double Node::calculatePromise() {
	const vector<double> &vec = (*observedClassDistribution);
	// get the max value index and sum of all values.
	double sum = 0.0f;
	unsigned int maxIndex = 0;
	for (unsigned int i = 0; i < vec.size(); i++) {
		sum += vec[i];
		if (vec[i] > vec[maxIndex]) {
			maxIndex = i;
		}
	}

	return sum - vec[maxIndex];
}

//void Node::addToValue(int index, double value) {
//	vector<double> &vec = (*observedClassDistribution);
//	if (  index >= vec.size()) {
//		vec.resize(index + 1, 0);
//	}
//	vec[index] += value;
//}

//double Node::sumOfValues() {
//	double sum = 0.0;
//	for (auto d : *observedClassDistribution) {
//		sum += d;
//	}
//	return sum;
//}

SplitNode::SplitNode(InstanceConditionalTest* splitTest,
		const vector<double>& classObservations, int size) :
		Node(classObservations) {
	this->splitTest = splitTest->clone();
	this->children = new list<Node*>(size, nullptr);
	this->childrenSize = size;
	this->mClassTypes = {NT_SplitNode, NT_Node};
}

SplitNode::SplitNode(InstanceConditionalTest* splitTest,
		const vector<double>& classObservations) :
		Node(classObservations) {
	this->splitTest = splitTest->clone();
	this->children = new list<Node*>;
	this->childrenSize = 0;
	this->mClassTypes = {NT_SplitNode, NT_Node};
}

SplitNode::SplitNode(const Json::Value& jv) :
		Node(jv) {
	this->children = new list<Node*>(jv["children"].size(), nullptr);
	this->childrenSize = jv["childrenSize"].asInt();
	int i = 0;
	for (auto iter = this->children->begin(); iter != this->children->end();
			iter++, i++) {
		string nodeType = jv["children"][i]["type"].asString();
		if (nodeType == "Node") {
			(*iter) = new Node(jv["children"][i]);
		} else if (nodeType == "SplitNode") {
			(*iter) = new SplitNode(jv["children"][i]);
		} else if (nodeType == "InactiveLearningNode") {
			(*iter) = new InactiveLearningNode(jv["children"][i]);
		} else if (nodeType == "ActiveLearningNode") {
			(*iter) = new ActiveLearningNode(jv["children"][i]);
		} else if (nodeType == "LearningNodeNB") {
			(*iter) = new LearningNodeNB(jv["children"][i]);
		} else if (nodeType == "LearningNodeNBAdaptive") {
			(*iter) = new LearningNodeNBAdaptive(jv["children"][i]);
		} else if (nodeType == "AdaSplitNode") {
			(*iter) = new AdaSplitNode(jv["children"][i]);
		} else if (nodeType == "AdaLearningNode") {
			(*iter) = new AdaLearningNode(jv["children"][i]);
		}
	}

	string splitTestType = jv["splitTest"]["type"].asString();
	if (splitTestType == "NumericAttributeBinaryTest") {
		this->splitTest = new NumericAttributeBinaryTest(jv["splitTest"]);
	} else if (splitTestType == "NominalAttributeMultiwayTest") {
		this->splitTest = new NominalAttributeMultiwayTest(jv["splitTest"]);
	} else if (splitTestType == "NominalAttributeBinaryTest") {
		this->splitTest = new NominalAttributeBinaryTest(jv["splitTest"]);
	}

	this->mClassTypes = {NT_SplitNode, NT_Node};
}

SplitNode::~SplitNode() {
	delete this->splitTest;
	auto iter = this->children->begin();
	for (; iter != this->children->end(); iter++) {
		if (*iter != nullptr) {
			delete (*iter);
		}
	}
	delete this->children;
}

void SplitNode::toJson(Json::Value& jv) {
	Node::toJson(jv);
	jv["type"] = "SplitNode";
	jv["childrenSize"] = this->childrenSize;
	if (splitTest == nullptr) {
		jv["splitTest"] = "nullptr";
	} else {
		splitTest->toJson(jv["splitTest"]);
	}

	if (children == nullptr) {
		jv["children"] = "nullptr";
	} else {
		int i = 0;
		for (auto n : *children) {
			n->toJson(jv["children"][i]);
			i++;
		}
	}
}

int SplitNode::numChildren() {
	return childrenSize;
}

void SplitNode::setChild(int index, Node* child) {
	if (this->splitTest->maxBranches() >= 0
			&& index >= this->splitTest->maxBranches()) {
		return;
	}

	if (index >= childrenSize) {
		children->resize(index + 1, nullptr);
		auto iter = children->end();
		iter--;
		(*iter) = child;
		childrenSize = index + 1;
	} else {
		auto iter = children->begin();
		advance(iter, index);
		(*iter) = child;
	}
}

Node* SplitNode::getChild(int index) {
	if ((index >= childrenSize) || (index < 0) ) {
		return nullptr;
	} else {
		auto iter = children->begin();
		advance(iter, index);
		return (*iter);
	}
}

int SplitNode::instanceChildIndex(const Instance* inst) {
	return splitTest->branchForInstance(inst);
}

bool SplitNode::isLeaf() {
	return false;
}

FoundNode* SplitNode::filterInstanceToLeaf(const Instance* inst,
		SplitNode* parent, int parentBranch) {
	int childIndex = instanceChildIndex(inst);
	if (childIndex >= 0) {
		Node* child = getChild(childIndex);
		if (child != nullptr) {
			return child->filterInstanceToLeaf(inst, this, childIndex);
		}
		return new FoundNode(nullptr, this, childIndex);
	}
	return new FoundNode(this, parent, parentBranch);
}

int SplitNode::subtreeDepth() {
	int maxChildDepth = 0;
	auto iter = children->begin();
	for (; iter != children->end(); iter++) {
		if ((*iter) != nullptr) {
			int depth = (*iter)->subtreeDepth();
			if (depth > maxChildDepth) {
				maxChildDepth = depth;
			}
		}
	}
	return maxChildDepth + 1;
}

LearningNode::LearningNode(const vector<double>& initialClassObservations) :
		Node(initialClassObservations) {
	this->mClassTypes = {NT_LearningNode, NT_Node};
}

LearningNode::LearningNode(const Json::Value& jv) :
		Node(jv) {
	this->mClassTypes = {NT_LearningNode, NT_Node};
}

InactiveLearningNode::InactiveLearningNode(
		const vector<double>& initialClassObservations) :
		LearningNode(initialClassObservations) {
	this->mClassTypes = {NT_InactiveLearningNode,NT_LearningNode,NT_Node};
}

InactiveLearningNode::InactiveLearningNode(const Json::Value& jv) :
		LearningNode(jv) {
	this->mClassTypes = {NT_InactiveLearningNode,NT_LearningNode,NT_Node};
}

void InactiveLearningNode::toJson(Json::Value& jv) {
	Node::toJson(jv);
	jv["type"] = "InactiveLearningNode";
}

void InactiveLearningNode::learnFromInstance(const Instance* inst,
		HoeffdingTree* ht) {
	Utils::addToValue(*this->observedClassDistribution, (int) inst->getLabel(),
			inst->getWeight());
}

ActiveLearningNode::ActiveLearningNode(
		const vector<double>& initialClassObservations) :
		LearningNode(initialClassObservations) {
	weightSeenAtLastSplitEvaluation = getWeightSeen();
	isInitialized = false;
	attrObsSize = 0;
	attributeObservers = new list<AttributeClassObserver*>;
	this->mClassTypes = {NT_ActiveLearningNode, NT_LearningNode, NT_Node};
}

ActiveLearningNode::ActiveLearningNode(const Json::Value& jv) :
		LearningNode(jv) {

	this->weightSeenAtLastSplitEvaluation =
			jv["weightSeenAtLastSplitEvaluation"].asDouble();
	this->attrObsSize = jv["attrObsSize"].asInt();
	this->isInitialized = jv["isInitialized"].asBool();

	if (jv["attributeObservers"].isArray()) {
		this->attributeObservers = new list<AttributeClassObserver*>(
				jv["attributeObservers"].size(), nullptr);
		int i = 0;
		for (auto iter = this->attributeObservers->begin();
				iter != this->attributeObservers->end(); iter++, i++) {
			string obsType = jv["attributeObservers"][i]["type"].asString();
			if (obsType == "NullAttributeClassObserver") {
				(*iter) = new NullAttributeClassObserver();
			} else if (obsType == "GaussianNumericAttributeClassObserver") {
				(*iter) = new GaussianNumericAttributeClassObserver(
						jv["attributeObservers"][i]);
			} else if (obsType == "NominalAttributeClassObserver") {
				(*iter) = new NominalAttributeClassObserver(
						jv["attributeObservers"][i]);
			}
		}
	} else {
		this->attributeObservers = new list<AttributeClassObserver*>;
	}

	this->mClassTypes = {NT_ActiveLearningNode, NT_LearningNode, NT_Node};
}

ActiveLearningNode::~ActiveLearningNode() {
	// clear data
	for (auto a : *attributeObservers) {
		if (a != nullptr) {
			delete a;
		}
	}
	// delete attributeObservers
	delete attributeObservers;
}

void ActiveLearningNode::toJson(Json::Value& jv) {
	Node::toJson(jv);
	jv["type"] = "ActiveLearningNode";

	jv["weightSeenAtLastSplitEvaluation"] =
			this->weightSeenAtLastSplitEvaluation;
	jv["attrObsSize"] = this->attrObsSize;
	jv["isInitialized"] = this->isInitialized;

	if (this->attributeObservers->size() == 0) {
		jv["attributeObservers"] = "nullptr";
	} else {
		int i = 0;
		for (auto iter = this->attributeObservers->begin();
				iter != this->attributeObservers->end(); iter++, i++) {
			if ((*iter) == nullptr) {
				jv["attributeObservers"][i] = "nullptr";
			} else {
				(*iter)->toJson(jv["attributeObservers"][i]);
			}
		}
	}
}

void ActiveLearningNode::learnFromInstance(const Instance* inst,
		HoeffdingTree* ht) {
	if (isInitialized == false) {
		attributeObservers->resize(inst->getNumberInputAttributes(), nullptr);
		attrObsSize = inst->getNumberInputAttributes();
		isInitialized = true;
	}

	Utils::addToValue(*this->observedClassDistribution, (int) inst->getLabel(),
			inst->getWeight());

	if (inst->getNumberInputAttributes() > this->attrObsSize) {
		attributeObservers->resize(inst->getNumberInputAttributes(), nullptr);
		attrObsSize = inst->getNumberInputAttributes();
	}

	auto iter = (*attributeObservers).begin();
	// TODO
//	for (int i = 0; i < attrObsSize; i++, iter++) {
	for (int i = 0; i < inst->getNumberInputAttributes(); i++, iter++) {
		int instAttIndex = i;
		AttributeClassObserver* obs = (*iter);
		if (obs == nullptr) {
			obs = inst->getInputAttribute(instAttIndex)->isNominal() ?
					ht->newNominalClassObserver() :
					ht->newNumericClassObserver();
			(*iter) = obs;
		}
		obs->observeAttributeClass(inst->getInputAttributeValue(instAttIndex),
				(int) inst->getLabel(), inst->getWeight());
	}
}

void ActiveLearningNode::showActiveLearningNode() {
	stringstream sb;

	// show observedClassDistribution
	sb << "ocd: ";
	for (unsigned int i = 0; i < observedClassDistribution->size(); i++) {
		sb << (*observedClassDistribution)[i];
		sb << " ";
	}
	sb << "\n";

	// show weightSeenAtLastSplitEvaluation
	sb << "wsalse: ";
	sb << weightSeenAtLastSplitEvaluation;
	sb << "\n";

	// show attributeObservers
	sb << "ao:\n";
	int i = 0;
	for (auto obs : (*attributeObservers)) {
		sb << i;
		sb << ": ";

		if (obs == nullptr) {
			sb << "null";
			sb << "\n";
		} else {
			if (i < 10) {
				GaussianNumericAttributeClassObserver* g =
						(GaussianNumericAttributeClassObserver*) obs;
				sb << "[";
				for (unsigned int j = 0; j < g->minValueObservedPerClass.size();
						j++) {
					sb << g->minValueObservedPerClass[j];
					sb << " ";
				}
				sb << "] ";
				sb << "[";
				for (unsigned int j = 0; j < g->maxValueObservedPerClass.size();
						j++) {
					sb << g->maxValueObservedPerClass[j];
					sb << " ";
				}
				sb << "] ";
				sb << "[";
				for (unsigned int j = 0; j < g->attValDistPerClass.size();
						j++) {
					GaussianEstimator* ge = g->attValDistPerClass[j];
					if (ge == nullptr) {
						sb << "null";
					} else {
						sb << ge->m_mean;
						sb << " ";
						sb << ge->m_varianceSum;
						sb << " ";
						sb << ge->m_weightSum;
					}
					sb << " | ";
				}
				sb << "] ";
				sb << "\n";
			} else {
				NominalAttributeClassObserver* n =
						(NominalAttributeClassObserver*) obs;
				sb << n->totalWeightObserved;
				sb << " ";
				sb << n->missingWeightObserved;
				sb << " [";
				auto iter = n->attValDistPerClass.begin();
				for (; iter != n->attValDistPerClass.end(); iter++) {
					if ((*iter).size() == 0) {
						sb << "null";
					} else {
						auto iter2 = (*iter).begin();
						for (; iter2 != (*iter).end(); iter2++) {
							sb << (*iter2);
							sb << " ";
						}
					}
					sb << " | ";
				}
				sb << "]";
				sb << "\n";
			}
		}
		i++;
	}

	cout << sb.str() << endl;
}

double ActiveLearningNode::getWeightSeen() const {
	return Utils::sum(*this->observedClassDistribution);
}

double ActiveLearningNode::getWeightSeenAtLastSplitEvaluation() {
	return weightSeenAtLastSplitEvaluation;
}

void ActiveLearningNode::setWeightSeenAtLastSplitEvaluation(double weight) {
	weightSeenAtLastSplitEvaluation = weight;
}

list<AttributeSplitSuggestion*>* ActiveLearningNode::getBestSplitSuggestions(
		SplitCriterion* criterion, HoeffdingTree* ht) {
	list<AttributeSplitSuggestion*>* bestSuggestions = new list<
			AttributeSplitSuggestion*>();
	vector<double>& preSplitDist = (*observedClassDistribution);

	if (!ht->params.noPrePrune) {
		vector<vector<double>> * vvd = new vector<vector<double>>(1);
		(*vvd)[0] = preSplitDist;

		bestSuggestions->push_back(
				new AttributeSplitSuggestion(nullptr,
						*(new vector<vector<double>>(0)),
						criterion->getMeritOfSplit(preSplitDist, *vvd)));
	}

	auto iter = attributeObservers->begin();
	for (int i = 0; iter != attributeObservers->end(); i++, iter++) {
		AttributeClassObserver* obs = (*iter);
		if (obs != nullptr) {
			AttributeSplitSuggestion* bestSuggestion =
					obs->getBestEvaluatedSplitSuggestion(criterion,
							preSplitDist, i, ht->params.binarySplits);
			if (bestSuggestion != nullptr) {
				bestSuggestions->push_back(bestSuggestion);
			}
		}
	}

	return bestSuggestions;
}

void ActiveLearningNode::disableAttribute(int attIndex) {
	list<AttributeClassObserver*> &obss = (*attributeObservers);
	auto iter = obss.begin();
	advance(iter, attIndex);
	delete *iter;
	*iter = new NullAttributeClassObserver();
}

LearningNodeNB::LearningNodeNB(const vector<double>& initialClassObservations) :
		ActiveLearningNode(initialClassObservations) {
	this->mClassTypes = {NT_LearningNodeNB,NT_ActiveLearningNode,NT_LearningNode,NT_Node};
}

LearningNodeNB::LearningNodeNB(const Json::Value& jv) :
		ActiveLearningNode(jv) {
	this->mClassTypes = {NT_LearningNodeNB,NT_ActiveLearningNode,NT_LearningNode,NT_Node};
}

vector<double>* LearningNodeNB::getClassVotes(const Instance* inst,
		HoeffdingTree* ht) {
	if (getWeightSeen() >= ht->params.nbThreshold) {
		auto pred = doNaiveBayesPrediction(inst,
				*this->observedClassDistribution, *this->attributeObservers);
		return pred;
	}
	return ActiveLearningNode::getClassVotes(inst, ht);
}

LearningNodeNB::~LearningNodeNB() {
}

void LearningNodeNB::toJson(Json::Value& jv) {
	ActiveLearningNode::toJson(jv);
	jv["type"] = "LearningNodeNB";
}

void LearningNodeNB::disableAttribute(int attIndex) {
//
}

vector<double>* LearningNodeNB::doNaiveBayesPrediction(const Instance* inst,
		const vector<double>& observedClassDistribution,
		const list<AttributeClassObserver*>& attributeObservers) {
	vector<double>* votes = &(this->classVotes);
	votes->resize(observedClassDistribution.size(), 0);

	double observedClassSum = Utils::sum(observedClassDistribution);
	for (unsigned int classIndex = 0;
			classIndex < observedClassDistribution.size(); classIndex++) {
		(*votes)[classIndex] = observedClassDistribution[classIndex]
				/ observedClassSum;
		auto iter = attributeObservers.begin();
		for (int attIndex = 0;
				attIndex < inst->getNumberInputAttributes()
						&& iter != attributeObservers.end();
				attIndex++, iter++) {
//			int instAttIndex = modelAttIndexToInstanceAttIndex(attIndex, inst);
			int instAttIndex = attIndex;
			AttributeClassObserver* obs = (*iter);
			double v = inst->getInputAttributeValue(instAttIndex);
			if ((obs != nullptr) && !std::isnan(v)) {
				(*votes)[classIndex] *=
						obs->probabilityOfAttributeValueGivenClass(v,
								classIndex);
			}
		}
	}

	return votes;
}

LearningNodeNBAdaptive::LearningNodeNBAdaptive(
		const vector<double>& initialClassObservations) :
		LearningNodeNB(initialClassObservations) {
	this->mcCorrectWeight = 0.0f;
	this->nbCorrectWeight = 0.0f;
	this->mClassTypes = {NT_LearningNodeNBAdaptive, NT_LearningNodeNB, NT_ActiveLearningNode, NT_LearningNode, NT_Node};
}

LearningNodeNBAdaptive::LearningNodeNBAdaptive(const Json::Value& jv) :
		LearningNodeNB(jv) {
	this->mcCorrectWeight = jv["mcCorrectWeight"].asDouble();
	this->nbCorrectWeight = jv["nbCorrectWeight"].asDouble();
	this->mClassTypes = {NT_LearningNodeNBAdaptive, NT_LearningNodeNB, NT_ActiveLearningNode, NT_LearningNode, NT_Node};
}

void LearningNodeNBAdaptive::toJson(Json::Value& jv) {
	LearningNodeNB::toJson(jv);
	jv["type"] = "LearningNodeNBAdaptive";
	jv["mcCorrectWeight"] = this->mcCorrectWeight;
	jv["nbCorrectWeight"] = this->nbCorrectWeight;
}

void LearningNodeNBAdaptive::learnFromInstance(const Instance* inst,
		HoeffdingTree* ht) {
	int trueClass = (int) inst->getLabel();
	if (Utils::maxIndex(*this->observedClassDistribution) == trueClass) {
		this->mcCorrectWeight += inst->getWeight();
	}
	vector<double>* pred = doNaiveBayesPrediction(inst,
			*this->observedClassDistribution, *this->attributeObservers);
	if (Utils::maxIndex(*pred) == trueClass) {
		this->nbCorrectWeight += inst->getWeight();
	}
	LearningNodeNB::learnFromInstance(inst, ht);
}

vector<double>* LearningNodeNBAdaptive::getClassVotes(const Instance* inst,
		HoeffdingTree* ht) {
	if (this->mcCorrectWeight > this->nbCorrectWeight) {
		return this->observedClassDistribution;
	}
	auto pred = doNaiveBayesPrediction(inst, *this->observedClassDistribution,
			*this->attributeObservers);
	return pred;
}

} /* namespace HT */

