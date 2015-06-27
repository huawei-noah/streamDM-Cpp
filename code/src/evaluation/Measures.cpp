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

#include "Measures.h"
#include <sstream>

Measures::Measures() {
}

Measures::~Measures() {
}

double Measures::getMeasure(int col, int row) {
	return data[row][col];
}

double Measures::getMeasure(int col) {
	// Get Last Measure
	return data[data.size()-1][col];
}

vector<double> Measures::getMeasures(int row) {
	return data[row];
}

void Measures::setMeasure(double value, int col, int row) {
	data[row][col] = value;
}

void Measures::addMeasure(double value, int row) {
	data[row].push_back(value);
}

void Measures::setMeasures(vector<double> measure, int row) {
	data[row] = measure;
}

void Measures::addMeasures(vector<double> measure) {
	data.push_back(measure);
}

string Measures::getName(int col) {
	return names[col];
}

void Measures::addName(string name, int col) {
	//names[col] = name;
	names.push_back(name);
}

int Measures::getNumberNames(){
	return names.size();
}

string Measures::lastMeasuresToString() {
    return "";
}

string Measures::toString() {
	std::stringstream s;
	s << endl;
	s << getName(0);
	for(int i = 1; i< getNumberNames(); i++) {
		s << "," << getName(i);
	}
	s << endl;
	for(int j = 0; j< data.size(); j++) {
		s << getMeasure(0,j);
		for(int i = 1; i< getNumberNames(); i++) {
			s << "," << getMeasure(i,j);
		}
		s << endl;
	}
	//s << "IntancesSeen: " << getMeasure(0) << endl;
	//s << " Accuracy: " << getMeasure(1) << endl;
	return s.str();
}
