#include "Majorityclass.h"
#include "../../../Common.h"

REGISTER_CLASS(MajorityClass);
REGISTER_COMMAND_LINE_PARAMETER(MajorityClass, "{\"type\":\"Learner\","
		"\"name\":\"MajorityClass\","
		"\"parameter\":{"
		"}}"
		"");

MajorityClass::MajorityClass() {
	//instancesCorrectlyClassified = 0;
	instancesSeen = 0;
	init = false;
	this->predArray = nullptr;
	this->classCounts = nullptr;
	this->predClassCount = 0;
}

MajorityClass::~MajorityClass() {
	if (init) {
		delete[] classCounts;
	}
	if (this->predArray != nullptr) {
		delete[] this->predArray;
	}
}

void MajorityClass::doSetParams() {
	//
}

void MajorityClass::train(const Instance& instance) {
	if (init == false) {
		//	std::cout << "$" << instance.getNumberClasses()<< "\n";
		classCounts = new int[instance.getNumberClasses()];
		//	 std::cout << "Init" << "\n";
		for (int i = 0; i < instance.getNumberClasses(); i++) {
			classCounts[i] = 0;
		}
		init = true;
	}
	//std::cout << "Adding" << "\n";
	int label = instance.getLabel();
	classCounts[label]++;
}
double* MajorityClass::getPrediction(const Instance& instance) {
//	if (this->predArray == nullptr) {
//		this->predClassCount = instance.getNumberClasses();
//		this->predArray = new double[this->predClassCount];
//	}else if (this->predClassCount != instance.getNumberClasses()){
//		delete[] this->predArray;
//		this->predClassCount = instance.getNumberClasses();
//		this->predArray = new double[this->predClassCount];
//	}
//
//	for (int i = 1; i < this->predClassCount; i++) {
//		this->predArray[i] = 0.0;
//	}
//	int pred = 0;
//	if (init) {
//		double max = classCounts[0];
//		for (int i = 1; i < this->predClassCount; i++) {
//			//std::cout << i <<"," << max <<"," <<  classCounts[i]<< "\n";
//			if (max < classCounts[i]) {
//				max = classCounts[i];
//				pred = i;
//			}
//		}
//	}
//	this->predArray[pred] = 1.0;
//	return this->predArray;

	double* result = new double[instance.getNumberClasses()];
	for (int i = 1; i < instance.getNumberClasses(); i++) {
		result[i] = 0.0;
	}
	int pred = 0;
	if (init) {
		double max = classCounts[0];
		for (int i = 1; i < instance.getNumberClasses(); i++) {
			if (max < classCounts[i]) {
				max = classCounts[i];
				pred = i;
			}
		}
	}
	result[pred] = 1.0;
	return result;
}
