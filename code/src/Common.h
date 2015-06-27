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

#ifndef COMMON_H_
#define COMMON_H_

#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <queue>
#include <stack>
#include <iterator>
#include <iostream>
#include <fstream> 
#include <sstream>  
#include <cmath>
#include <ctime>
#include <algorithm>
#include <functional>
#include <utility>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <unistd.h>

#include "utils/Log.h"
#include "utils/Utils.h"
#include "utils/json.h"
#include "utils/Configurable.h"
#include "utils/DynamicCreateClass.h"
#include "utils/CommandLineParser.h"
#include "utils/RTTI.h"

#include "core/Attribute.h"
#include "core/DenseInstance.h"
#include "core/DiscreteEstimator.h"
#include "core/DoubleVector.h"
#include "core/GaussianEstimator.h"
#include "core/Instance.h"
#include "core/InstanceInformation.h"
#include "core/SparseEstimator.h"
#include "core/SparseInstance.h"
#include "core/SplitCriterion.h"

#include "tasks/Task.h"
#include "streams/Reader.h"
#include "learners/Learner.h"

// #define max(A,B) ((A)>(B) ? (A) : (B))
// #define min(A,B) ((A)>(B) ? (B) : (A))
#define lg(x)  (log(x) / log(2.0))
//#define isnan(x) __builtin_isnan(x)

using namespace std;

#endif /* COMMON_H_ */
