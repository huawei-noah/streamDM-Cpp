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

#ifndef UTILS_H_
#define UTILS_H_

#include <ctime>

#include <list>
#include <vector>
#include <map>
#include <cmath>
#include <iostream>

using namespace std;

class Utils {
public:
	Utils();
	static int poisson(double lambda);
	static double elapsedTime(clock_t start);

	template<class Type>
	static int maxIndex(Type& a);

	static void addToValue(list<double>& a, int index, double v);
	static void addToValue(vector<double>& a, int index, double v);

	template<class Type>
	static double sum(Type& a);

	template<class Type>
	static void normalize(Type& a, double sum);

	template<class Type>
	static int numNonZeroEntries(Type &a);

	static bool checkFileExist(const string& fileName);

	static long DJBHash(const list<int>&);

	static bool getFileLastModifiedTime(const string& path, time_t& time);
	static size_t getFileLength(const string& path);

private:
	static bool init;
};

template<class Type>
inline void Utils::normalize(Type& a, double sum) {
	if (!std::isnan(sum) && sum != 0) {
			for (auto iter = a.begin(); iter != a.end(); iter++) {
				(*iter) /= sum;
			}
	}
}

template<class Type>
inline int Utils::numNonZeroEntries(Type &a) {
	int count = 0;
	for (auto e : a) {
		if (e != 0.0) {
			count++;
		}
	}
	return count;
}

template<class Type>
inline int Utils::maxIndex(Type &a) {
	int i = 0;
	int maxIndex = 0;
	double maxValue = -1 * INFINITY;
	for (auto d : a) {
		if (!std::isnan(d) && d > maxValue) {
			maxIndex = i;
			maxValue = d;
		}
		i++;
	}
	return maxIndex;
}

template<class Type>
inline double Utils::sum(Type &a) {
	double sv = 0.0;
	for (auto d : a) {
		sv += d;
	}
	return sv;
}

#endif /* UTILS_H_ */
