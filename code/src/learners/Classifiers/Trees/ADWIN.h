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

#ifndef ADWIN_H_
#define ADWIN_H_

#include <vector>
#include <list>
#include "../../../Common.h"

using namespace std;

namespace HT {

class ADListItem {
public:
	ADListItem* nextItem;
	ADListItem* previousItem;
	int bucketSizeRow;
	int MAXBUCKETS;
	vector<double> bucketTotal;
	vector<double> bucketVariance;

	ADListItem();
	ADListItem(ADListItem* nextNode, ADListItem* previousNode);
	ADListItem(const Json::Value& jv);
	void init(ADListItem* nextNode, ADListItem* previousNode);
	void clear();
	void clearBucket(int k);
	void insertBucket(double Value, double Variance);
	void RemoveBucket();
	void compressBucketsRow(int NumberItemsDeleted);
	ADListItem* previous();
	void setPrevious(ADListItem* previous);
	ADListItem* next();
	void setNext(ADListItem* next);
	double Total(int k);
	double Variance(int k);
	void setTotal(double value, int k);
	void setVariance(double value, int k);

	void toJson(Json::Value& jv);
};

class ADList {
public:
	int count;
	ADListItem* headItem;
	ADListItem* tailItem;

	ADList();
	ADList(const Json::Value& jv);
	int size();
	ADListItem* head();
	ADListItem* tail();
	bool isEmpty();
	void clear();
	void addToHead();
	void removeFromHead();
	void addToTail();
	void removeFromTail();

	void toJson(Json::Value& jv);
};

class ADWIN {
public:
	ADWIN();
	ADWIN(double d);
	ADWIN(int cl);
	ADWIN(const Json::Value& jv);
	virtual ~ADWIN();
	void init();

	static const double DELTA; //.1;
	static const int mintMinimLongitudWindow; //10
	double mdbldelta; //.1;
	int mintTime;
	int mintClock;
	double mdblWidth; // Mean of Width = mdblWidth/Number of items
	static const int MAXBUCKETS;
	int lastBucketRow;
	double TOTAL;
	double VARIANCE;
	int WIDTH;
	int BucketNumber;
	int Detect;
	int numberDetections;
	int DetectTwice;
	bool blnBucketDeleted;
	int BucketNumberMAX;
	int mintMinWinLength;

	ADList* listRowBuckets;

	bool getChange();
	void resetChange();
	int getBucketsUsed();
	int getWidth();
	void setClock(int intClock);
	int getClock();
	bool getWarning();
	bool getDetect();
	int getNumberDetections();
	double getTotal();
	double getEstimation();
	double getVariance();
	double getWidthT();
	void initBuckets();
	void insertElement(double Value);
	void insertElementBucket(double Variance, double Value, ADListItem* Node);
	int bucketSize(int Row);
	int deleteElement();
	void compressBuckets();
	bool setInput(double intEntrada);
	bool setInput(double intEntrada, double delta);
	bool blnCutexpression(int n0, int n1, double u0, double u1, double v0,
			double v1, double absvalue, double delta);
	void setW(int W0);

	void toJson(Json::Value& jv);

};

} /* namespace HT */

#endif /* ADWIN_H_ */
