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

#include "ADWIN.h"
#include <cmath>
#include <iostream>

using namespace std;

namespace HT {

ADListItem::ADListItem() {
	init(nullptr, nullptr);
}

ADListItem::ADListItem(ADListItem* nextNode, ADListItem* previousNode) {
	init(nextNode, previousNode);
}

ADListItem::ADListItem(const Json::Value& jv) {
	this->bucketSizeRow = jv["bucketSizeRow"].asInt();
	this->MAXBUCKETS = jv["MAXBUCKETS"].asInt();

	if (jv["bucketTotal"].isString()) {
		this->bucketTotal.resize(0);
	} else {
		int bucketTotalSize = jv["bucketTotal"].size();
		this->bucketTotal.resize(bucketTotalSize);
		for (int i = 0; i < bucketTotalSize; i++) {
			this->bucketTotal[i] = jv["bucketTotal"][i].asDouble();
		}
	}

	if (jv["bucketVariance"].isString()) {
		this->bucketVariance.resize(0);
	} else {
		int bucketVarianceSize = jv["bucketVariance"].size();
		this->bucketVariance.resize(bucketVarianceSize);
		for (int i = 0; i < bucketVarianceSize; i++) {
			this->bucketVariance[i] = jv["bucketVariance"][i].asDouble();
		}
	}

	this->previousItem = nullptr;
	this->nextItem = nullptr;
}

void ADListItem::toJson(Json::Value& jv) {
	jv["type"] = "ADListItem";
	jv["bucketSizeRow"] = this->bucketSizeRow;
	jv["MAXBUCKETS"] = this->MAXBUCKETS;

	int bucketTotalSize = this->bucketTotal.size();
	if (bucketTotalSize == 0) {
		jv["bucketTotal"] = "empty";
	} else {
		for (int i = 0; i < bucketTotalSize; i++) {
			jv["bucketTotal"][i] = this->bucketTotal[i];
		}
	}

	int bucketVarianceSize = this->bucketVariance.size();
	if (bucketVarianceSize == 0) {
		jv["bucketVariance"] = "empty";
	} else {
		for (int i = 0; i < bucketVarianceSize; i++) {
			jv["bucketVariance"][i] = this->bucketVariance[i];
		}
	}

	if (this->previousItem == nullptr) {
		jv["previousItem"] = "nullptr";
	} else if (this->previousItem == this) {
		jv["previousItem"] = "self";
	} else {
		jv["previousItem"] = "previousItem";
	}

	if (this->nextItem == nullptr) {
		jv["nextItem"] = "nullptr";
	} else if (this->nextItem == this) {
		jv["nextItem"] = "self";
	} else {
		jv["nextItem"] = "nextItem";
	}
}

void ADListItem::init(ADListItem* nextNode, ADListItem* previousNode) {
	this->bucketSizeRow = 0;
	this->MAXBUCKETS = ADWIN::MAXBUCKETS;
	this->bucketTotal.resize(MAXBUCKETS + 1);
	this->bucketVariance.resize(MAXBUCKETS + 1);

	this->nextItem = nextNode;
	this->previousItem = previousNode;
	if (nextNode != nullptr) {
		nextNode->previousItem = this;
	}
	if (previousNode != nullptr) {
		previousNode->nextItem = this;
	}
	clear();
}

void ADListItem::clear() {
	bucketSizeRow = 0;
	for (int k = 0; k <= MAXBUCKETS; k++) {
		clearBucket(k);
	}
}

void ADListItem::clearBucket(int k) {
	setTotal(0, k);
	setVariance(0, k);
}

void ADListItem::insertBucket(double Value, double Variance) {
	int k = bucketSizeRow;
	bucketSizeRow++;
	//Insert new bucket
	setTotal(Value, k);
	setVariance(Variance, k);
}

void ADListItem::RemoveBucket() {
	compressBucketsRow(1);
}

void ADListItem::compressBucketsRow(int NumberItemsDeleted) {
	for (int k = NumberItemsDeleted; k <= MAXBUCKETS; k++) {
		bucketTotal[k - NumberItemsDeleted] = bucketTotal[k];
		bucketVariance[k - NumberItemsDeleted] = bucketVariance[k];
	}
	for (int k = 1; k <= NumberItemsDeleted; k++) {
		clearBucket(MAXBUCKETS - k + 1);
	}
	bucketSizeRow -= NumberItemsDeleted;
}

ADListItem* ADListItem::previous() {
	return this->previousItem;
}

void ADListItem::setPrevious(ADListItem* previous) {
	if (this->previousItem != nullptr) {
		delete this->previousItem;
	}
	this->previousItem = previous;
}

ADListItem* ADListItem::next() {
	return this->nextItem;
}

void ADListItem::setNext(ADListItem* next) {
	if (this->nextItem != nullptr) {
		delete this->nextItem;
	}
	this->nextItem = next;
}

double ADListItem::Total(int k) {
	return bucketTotal[k];
}

double ADListItem::Variance(int k) {
	return bucketVariance[k];
}

void ADListItem::setTotal(double value, int k) {
	bucketTotal[k] = value;
}

void ADListItem::setVariance(double value, int k) {
	bucketVariance[k] = value;
}

ADList::ADList() {
	this->headItem = nullptr;
	this->tailItem = nullptr;
	this->count = 0;
	addToHead();
}

ADList::ADList(const Json::Value& jv) {
	this->count = jv["count"].asInt();
	if (this->count == 0) {
		this->headItem = nullptr;
		this->tailItem = nullptr;
	} else if (this->count == 1) {
		this->headItem = new ADListItem(jv["items"][0]);
		this->tailItem = this->headItem;
	} else {
		this->headItem = new ADListItem(jv["items"][0]);
		ADListItem* old = this->headItem;
		for (int i = 1; i < this->count; i++) {
			ADListItem* current = new ADListItem(jv["items"][i]);
			old->nextItem = current;
			current->previousItem = old;
			old = current;
		}
		this->tailItem = old;
	}
}

void ADList::toJson(Json::Value& jv) {

	jv["type"] = "ADList";
	jv["count"] = this->count;

	if (this->headItem == nullptr || this->tailItem == nullptr) {
		jv["items"] = "emplty";
		if (this->headItem != nullptr || this->tailItem != nullptr) {
			LOG_ERROR(
					"ADList, the headItem and tailItem are not both nullptr.");
		}
	} else {
		ADListItem* item = this->headItem;
		int i = 0;
		while (item != this->tailItem && item != nullptr) {
			item->toJson(jv["items"][i]);
			i++;
			item = item->next();
		}
		if (item == this->tailItem) {
			item->toJson(jv["items"][i]);
		}

	}

//	ADListItem* headItem;
//	ADListItem* tailItem;
}

int ADList::size() {
	return this->count;
}

ADListItem* ADList::head() {
	return this->headItem;
}

ADListItem* ADList::tail() {
	return this->tailItem;
}

bool ADList::isEmpty() {
	return this->count == 0;
}

void ADList::clear() {
	if (this->headItem != nullptr) {
		delete this->headItem;
		this->headItem = nullptr;
	}
	if (this->tailItem != nullptr) {
		delete this->tailItem;
		this->tailItem = nullptr;
	}
	this->count = 0;
}

void ADList::addToHead() {
	this->headItem = new ADListItem(this->headItem, nullptr);
	if (this->tailItem == nullptr) {
		this->tailItem = this->headItem;
	}
	this->count++;
}

void ADList::removeFromHead() {
	ADListItem *item = this->headItem;
	this->headItem = this->headItem->next();
	delete item;
	if (this->headItem != nullptr) {
		this->headItem->setPrevious(nullptr);
	} else {
		this->tailItem = nullptr;
	}
	this->count--;
	return;
}

void ADList::addToTail() {
	this->tailItem = new ADListItem(nullptr, this->tailItem);
	if (this->headItem == nullptr) {
		this->headItem = this->tailItem;
	}
	this->count++;
}

void ADList::removeFromTail() {
//	ADListItem* item = this->tailItem;
	this->tailItem = this->tailItem->previous();
	// TODO why not delete item?
//	delete item;
	if (this->tailItem == nullptr) {
		this->headItem = nullptr;
	} else {
		this->tailItem->setNext(nullptr);
	}
	this->count--;
	return;
}

const double ADWIN::DELTA = .002f; //.1;
const int ADWIN::mintMinimLongitudWindow = 10; //10
const int ADWIN::MAXBUCKETS = 5;

ADWIN::ADWIN() {
	init();
}

ADWIN::ADWIN(double d) {
	init();
	mdbldelta = d;
}

ADWIN::ADWIN(int cl) {
	init();
	mintClock = cl;
}

ADWIN::ADWIN(const Json::Value& jv) {
	this->mdbldelta = jv["mdbldelta"].asDouble();
	this->mintTime = jv["mintTime"].asInt();
	this->mintClock = jv["mintClock"].asInt();
	this->mdblWidth = jv["mdblWidth"].asDouble();
	this->lastBucketRow = jv["lastBucketRow"].asInt();
	this->TOTAL = jv["TOTAL"].asDouble();
	this->VARIANCE = jv["VARIANCE"].asDouble();
	this->WIDTH = jv["WIDTH"].asInt();
	this->BucketNumber = jv["BucketNumber"].asInt();
	this->Detect = jv["Detect"].asInt();
	this->numberDetections = jv["numberDetections"].asInt();
	this->DetectTwice = jv["DetectTwice"].asInt();
	this->blnBucketDeleted = jv["blnBucketDeleted"].asBool();
	this->BucketNumberMAX = jv["BucketNumberMAX"].asInt();
	this->mintMinWinLength = jv["mintMinWinLength"].asInt();

	if (jv["listRowBuckets"].isString()){
		this->listRowBuckets = nullptr;
	}else{
		this->listRowBuckets = new ADList(jv["listRowBuckets"]);
	}
}

void ADWIN::toJson(Json::Value& jv) {
	jv["mdbldelta"] = this->mdbldelta;
	jv["mintTime"] = this->mintTime;
	jv["mintClock"] = this->mintClock;
	jv["mdblWidth"] = this->mdblWidth;
	jv["lastBucketRow"] = this->lastBucketRow;
	jv["TOTAL"] = this->TOTAL;
	jv["VARIANCE"] = this->VARIANCE;
	jv["WIDTH"] = this->WIDTH;
	jv["BucketNumber"] = this->BucketNumber;
	jv["Detect"] = this->Detect;
	jv["numberDetections"] = this->numberDetections;
	jv["DetectTwice"] = this->DetectTwice;
	jv["blnBucketDeleted"] = this->blnBucketDeleted;
	jv["BucketNumberMAX"] = this->BucketNumberMAX;
	jv["mintMinWinLength"] = this->mintMinWinLength;

	if (this->listRowBuckets == nullptr) {
		jv["listRowBuckets"] = "nullptr";
	} else {
		this->listRowBuckets->toJson(jv["listRowBuckets"]);
	}
}

ADWIN::~ADWIN() {
}

void ADWIN::init() {
	mdbldelta = .002; //.1;
	mintTime = 0;
	mintClock = 32;
	mdblWidth = 0; // Mean of Width = mdblWidth/Number of items
	lastBucketRow = 0;
	TOTAL = 0;
	VARIANCE = 0;
	WIDTH = 0;
	BucketNumber = 0;
	Detect = 0;
	numberDetections = 0;
	DetectTwice = 0;
	blnBucketDeleted = false;
	BucketNumberMAX = 0;
	mintMinWinLength = 5;

	mdbldelta = DELTA;
	initBuckets();
	Detect = 0;
	numberDetections = 0;
	DetectTwice = 0;
}

void ADWIN::initBuckets() {
	listRowBuckets = new ADList();
	lastBucketRow = 0;
	TOTAL = 0;
	VARIANCE = 0;
	WIDTH = 0;
	BucketNumber = 0;
}

bool ADWIN::getChange() {
	return blnBucketDeleted;
}

void ADWIN::resetChange() {
	blnBucketDeleted = false;
}

int ADWIN::getBucketsUsed() {
	return BucketNumberMAX;
}

int ADWIN::getWidth() {
	return WIDTH;
}

void ADWIN::setClock(int intClock) {
	mintClock = intClock;
}

int ADWIN::getClock() {
	return mintClock;
}

bool ADWIN::getWarning() {
	return false;
}

bool ADWIN::getDetect() {
	return (Detect == mintTime);
}

int ADWIN::getNumberDetections() {
	return numberDetections;
}

double ADWIN::getTotal() {
	return TOTAL;
}

double ADWIN::getEstimation() {
	return TOTAL / WIDTH;
}

double ADWIN::getVariance() {
	return VARIANCE / WIDTH;
}

double ADWIN::getWidthT() {
	return mdblWidth;
}

void ADWIN::insertElement(double Value) {
	WIDTH++;
	insertElementBucket(0, Value, listRowBuckets->head());
	double incVariance = 0;
	if (WIDTH > 1) {
		incVariance = (WIDTH - 1) * (Value - TOTAL / (WIDTH - 1))
				* (Value - TOTAL / (WIDTH - 1)) / WIDTH;
	}
	VARIANCE += incVariance;
	TOTAL += Value;
	compressBuckets();
}

void ADWIN::insertElementBucket(double Variance, double Value,
		ADListItem* Node) {
	//Insert new bucket
	Node->insertBucket(Value, Variance);
	BucketNumber++;
	if (BucketNumber > BucketNumberMAX) {
		BucketNumberMAX = BucketNumber;
	}
}

int ADWIN::bucketSize(int Row) {
	return (int) pow(2, Row);
}

int ADWIN::deleteElement() {        //LIST
	//Update statistics
	ADListItem* node;
	node = listRowBuckets->tail();
	int n1 = bucketSize(lastBucketRow);
	WIDTH -= n1;
	TOTAL -= node->Total(0);
	double u1 = node->Total(0) / n1;
	double incVariance = node->Variance(0)
			+ n1 * WIDTH * (u1 - TOTAL / WIDTH) * (u1 - TOTAL / WIDTH)
					/ (n1 + WIDTH);
	VARIANCE -= incVariance;

	//Delete Bucket
	node->RemoveBucket();
	BucketNumber--;
	if (node->bucketSizeRow == 0) {
		listRowBuckets->removeFromTail();
		lastBucketRow--;
	}
	return n1;
}

void ADWIN::compressBuckets() { //Traverse the list of buckets in increasing order
	int n1, n2;
	double u2, u1, incVariance;
	ADListItem* cursor;
	ADListItem* nextNode;
	cursor = listRowBuckets->head();
	int i = 0;
	do {
		//Find the number of buckets in a row
		int k = cursor->bucketSizeRow;
		//If the row is full, merge buckets
		if (k == MAXBUCKETS + 1) {
			nextNode = cursor->next();
			if (nextNode == nullptr) {
//				cout << "nextNode == nullptr 1: " << (nextNode==nullptr) << endl;
				listRowBuckets->addToTail();
				nextNode = cursor->next();
//				cout << "nextNode == nullptr 2: " << (nextNode==nullptr) << endl;
				lastBucketRow++;
			}
			n1 = bucketSize(i);
			n2 = bucketSize(i);
			u1 = cursor->Total(0) / n1;
			u2 = cursor->Total(1) / n2;
			incVariance = n1 * n2 * (u1 - u2) * (u1 - u2) / (n1 + n2);

			// FIXME nextNode is null
			nextNode->insertBucket(cursor->Total(0) + cursor->Total(1),
					cursor->Variance(0) + cursor->Variance(1) + incVariance);
			BucketNumber++;
			cursor->compressBucketsRow(2);
			if (nextNode->bucketSizeRow <= MAXBUCKETS) {
				break;
			}
		} else {
			break;
		}
		cursor = cursor->next();
		i++;
	} while (cursor != nullptr);
}

bool ADWIN::setInput(double intEntrada) {
	return setInput(intEntrada, mdbldelta);
}

bool ADWIN::setInput(double intEntrada, double delta) {
	bool blnChange = false;
	bool blnExit = false;
	ADListItem* cursor;
	mintTime++;

	//1,2)Increment window in one element
	insertElement(intEntrada);
	blnBucketDeleted = false;
	//3)Reduce  window
	if (mintTime % mintClock == 0 && getWidth() > mintMinimLongitudWindow) {
		bool blnReduceWidth = true; // Diference

		while (blnReduceWidth) // Diference
		{
			blnReduceWidth = false; // Diference
			blnExit = false;
			int n0 = 0;
			int n1 = WIDTH;
			double u0 = 0;
			double u1 = getTotal();
			double v0 = 0;
			double v1 = VARIANCE;
			double n2 = 0;
			double u2 = 0;

			cursor = listRowBuckets->tail();
			int i = lastBucketRow;
			do {
				for (int k = 0; k <= (cursor->bucketSizeRow - 1); k++) {
					n2 = bucketSize(i);
					u2 = cursor->Total(k);
					if (n0 > 0) {
						v0 += cursor->Variance(k)
								+ (double) n0 * n2 * (u0 / n0 - u2 / n2)
										* (u0 / n0 - u2 / n2) / (n0 + n2);
					}
					if (n1 > 0) {
						v1 -= cursor->Variance(k)
								+ (double) n1 * n2 * (u1 / n1 - u2 / n2)
										* (u1 / n1 - u2 / n2) / (n1 + n2);
					}

					n0 += bucketSize(i);
					n1 -= bucketSize(i);
					u0 += cursor->Total(k);
					u1 -= cursor->Total(k);

					if (i == 0 && k == cursor->bucketSizeRow - 1) {
						blnExit = true;
						break;
					}
					double absvalue = (double) (u0 / n0) - (u1 / n1); //n1<WIDTH-mintMinWinLength-1
					if ((n1 > mintMinWinLength + 1 && n0 > mintMinWinLength + 1)
							&& // Diference NEGATIVE
								//if(
							blnCutexpression(n0, n1, u0, u1, v0, v1, absvalue,
									delta)) {
						blnBucketDeleted = true;
						Detect = mintTime;

						if (Detect == 0) {
							Detect = mintTime;
							//blnFirst=true;
							//blnWarning=true;
						} else if (DetectTwice == 0) {
							DetectTwice = mintTime;
							//blnDetect=true;
						}
						blnReduceWidth = true; // Diference
						blnChange = true;
						if (getWidth() > 0) { //Reduce width of the window
							//while (n0>0)  // Diference NEGATIVE
							n0 -= deleteElement();
							blnExit = true;
							break;
						}
					} //End if
				} //Next k
				cursor = cursor->previous();
				i--;
			} while (((!blnExit && cursor != nullptr)));
		} //End While // Diference
	} //End if

	mdblWidth += getWidth();
	if (blnChange) {
		numberDetections++;
	}
	return blnChange;
}

bool ADWIN::blnCutexpression(int n0, int n1, double u0, double u1, double v0,
		double v1, double absvalue, double delta) {
	int n = getWidth();
	double dd = log(2 * log(n) / delta); // -- ull perque el ln n va al numerador.
	// Formula Gener 2008
	double v = getVariance();
	double m = ((double) 1 / ((n0 - mintMinWinLength + 1)))
			+ ((double) 1 / ((n1 - mintMinWinLength + 1)));
	double epsilon = sqrt(2 * m * v * dd) + (double) 2 / 3 * dd * m;

	return (fabs(absvalue) > epsilon);
}

void ADWIN::setW(int W0) {
	//
}

} /* namespace HT */

