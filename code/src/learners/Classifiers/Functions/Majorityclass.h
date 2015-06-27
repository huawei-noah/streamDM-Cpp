#ifndef MAJORITYCLASS_H
#define MAJORITYCLASS_H

#include "../../Learner.h"

class MajorityClass: public Learner {
public:
	MajorityClass();
	~MajorityClass();
	void train(const Instance&);
	double* getPrediction(const Instance&);
private:
	int* classCounts;
	double* predArray;
	int predClassCount;
	void doSetParams();
};

#endif //MAJORITYCLASS_H