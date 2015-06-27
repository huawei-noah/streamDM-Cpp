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

#ifndef MEASURES_H_
#define MEASURES_H_
#include <vector>
#include <string>
#include <string.h>
using namespace std;

class Measures {
public:
	Measures();
	virtual ~Measures();
	double getMeasure(int, int);
	double getMeasure(int);
	vector<double> getMeasures(int);

	void setMeasure(double, int, int);
	void setMeasures(vector<double>, int);
	void addMeasure(double, int);
	void addMeasures(vector<double>);
	string getName(int);
	void addName(string, int);
	int getNumberNames();
	string lastMeasuresToString();
	string toString();

protected:
	vector<string> names;
	vector<vector<double> > data;
};

#endif /* MEASURES_H_ */
