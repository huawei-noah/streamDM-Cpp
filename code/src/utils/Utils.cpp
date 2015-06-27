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

#include "Utils.h"
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <unistd.h>
#include <cmath>

#include <sys/types.h>
#include <sys/stat.h>

bool Utils::init = false;
Utils utils;

Utils::Utils() {
	if (!init) {
		init = true;
		srand(time(NULL));
	}
}

int Utils::poisson(double lambda) {
	double product = 1.0;
	double sum = 1.0;
	double nextDouble = (double) (rand() % 1000) / 1000.0;
	double threshold = nextDouble * exp(lambda);
	int i = 1;
	int max = std::max(100, 10 * (int) (lambda));
	while ((i < max) && (sum <= threshold)) {
		product *= (lambda / i);
		sum += product;
		i++;
	}
	return i - 1;
}

double Utils::elapsedTime(clock_t start) {
	return static_cast<double>(clock() - start) / CLOCKS_PER_SEC;
}

void Utils::addToValue(list<double>& a, int index, double v) {
	int size = a.size();
	if (index >= size) {
		a.resize(index + 1, 0.0);
		auto iter = a.end();
		iter--;
		(*iter) = v;
	} else {
		auto iter = a.begin();
		advance(iter, index);
		(*iter) += v;
	}
}

void Utils::addToValue(vector<double>& a, int index, double v) {
	if (index >= a.size()) {
		a.resize(index + 1, 0.0);
	}
	a[index] += v;
}

bool Utils::checkFileExist(const string& fileName) {
	if (access(fileName.c_str(), 0) != 0) {
		return false;
	}
	return true;
}

long Utils::DJBHash(const list<int>& data) {
	long hash = 5381;
	for (auto iter = data.begin(); iter != data.end(); iter++) {
		hash = ((hash << 5) + hash) + (*iter);
	}
	return hash;
}

bool Utils::getFileLastModifiedTime(const string& path, time_t& time) {
	struct stat myStat;
	if (stat(path.c_str(), &myStat) < 0) {
		return false;
	}
	time = myStat.st_mtime;
	return true;
}

size_t Utils::getFileLength(const string& path) {
	struct stat myStat;
	if (stat(path.c_str(), &myStat) == 0) {
		return myStat.st_size;
	}
	return 0;
}
