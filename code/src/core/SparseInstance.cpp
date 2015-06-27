//================================================================================//
// Copyright 2009 Google Inc.                                                     //
//                                                                                // 
// Licensed under the Apache License, Version 2.0 (the "License");                //
// you may not use this file except in compliance with the License.               //
// You may obtain a copy of the License at                                        //
//                                                                                //
//      http://www.apache.org/licenses/LICENSE-2.0                                //
//                                                                                //
// Unless required by applicable law or agreed to in writing, software            //
// distributed under the License is distributed on an "AS IS" BASIS,              //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.       //
// See the License for the specific language governing permissions and            //
// limitations under the License.                                                 //
//================================================================================//
//
//
// Implementation of sf-sparse-vector.h

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include "SparseInstance.h"
#include "../Common.h"

//----------------------------------------------------------------//
//---------------- SfSparseVector Public Methods ----------------//
//----------------------------------------------------------------//
SparseInstance::SparseInstance(const char* in_string)
    : m_y(0.0), 
    m_a(0.0),
    m_squared_norm(0.0),
    m_group_id("") 
{
    noBias();
    init(in_string);
}

SparseInstance::SparseInstance(const char* in_string,
                               bool use_bias_term)
                               :m_y(0.0), m_a(0.0), m_squared_norm(0.0), m_group_id("") 
{
    if (use_bias_term) 
    {
        setBias();
    } else 
    {
        noBias();
    }
    init(in_string);
}

SparseInstance::SparseInstance(const SparseInstance& a,
                               const SparseInstance& b,
                               float y) 
                               : m_y(y),
                               m_a(0.0),
                               m_squared_norm(0.0) 
{
    m_group_id = a.getGroupId();
    int a_i = 0;
    int b_i = 0;
    while (a_i < a.numFeatures() || b_i < b.numFeatures()) {
        // a has no features remaining.
        if (!(a_i < a.numFeatures())) {
            pushPair(b.getInputAttributeID(b_i), 0.0 - b.getInputAttributeValue(b_i));
            ++b_i;
            continue;
        }
        // b has no features remaining.
        if (!(b_i < b.numFeatures())) {
            pushPair(a.getInputAttributeID(a_i), a.getInputAttributeValue(a_i));
            ++a_i;
            continue;
        }
        // a's current feature is less than b's current feature.
        if (b.getInputAttributeID(b_i) < a.getInputAttributeID(a_i)) {
            pushPair(b.getInputAttributeID(b_i), 0.0 - b.getInputAttributeValue(b_i));
            ++b_i;
            continue;
        }
        // b's current feature is less than a's current feature.
        if (a.getInputAttributeID(a_i) < b.getInputAttributeID(b_i)) {
            pushPair(a.getInputAttributeID(a_i), a.getInputAttributeValue(a_i));
            ++a_i;
            continue;
        }
        // a_i and b_i are pointing to the same feature.
        pushPair(a.getInputAttributeID(a_i), a.getInputAttributeValue(a_i) - b.getInputAttributeValue(b_i));
        ++a_i;
        ++b_i;
    }
}

Json::Value SparseInstance::toJson() const
{
	Json::Value jv;
	jv["m_y"] = m_y;
    for (int i = 0; i < numFeatures(); ++i) {
    	jv["attributes"][getInputAttributeID(i)] =  getInputAttributeValue(i);
    }
    if (!m_comment.empty()) {
    	jv["m_comment"] =  m_comment;
    }
    return jv;
}

void SparseInstance::pushPair(int id, float value) 
{
    if (id > 0 && numFeatures() > 0 && id <= getInputAttributeID(numFeatures() - 1) ) {
        std::cerr << id << " vs. " << getInputAttributeID(numFeatures() - 1) << std::endl;
        dieFormat("Features not in ascending sorted order.");
    }

    FeatureValuePair feature_value_pair;
    feature_value_pair.id_ = id;
    feature_value_pair.value_ = value;
    m_features.push_back(feature_value_pair);
    m_squared_norm += value * value;
}

//----------------------------------------------------------------//
//--------------- SfSparseVector Private Methods ----------------//
//----------------------------------------------------------------//

void SparseInstance::dieFormat(const string& reason) {
    std::cerr << "Wrong format for input data:\n  " << reason << std::endl;
    exit(1);
}

void SparseInstance::init(const char* in_string) 
{
    int length = strlen(in_string);
    if (length == 0) dieFormat("Empty example string.");

    // Get class label.
    if (!sscanf(in_string, "%f", &m_y))
        dieFormat("Class label must be real number.");

    // Parse the group id, if any.
    const char* position;
    position = strchr(in_string, ' ') + 1;
    if ((position[0] >= 'a' && position[0] <= 'z') ||
        (position[0] >= 'A' && position[0] <= 'Z')) {
            position = strchr(position, ':') + 1;
            const char* end = strchr(position, ' ');
            char group_id_c_string[1000];
            strncpy(group_id_c_string, position, end - position);
            m_group_id = group_id_c_string;
            position = end + 1;
    } 

    // Get feature:value pairs.
    for ( ;
        (position < in_string + length 
        && position - 1 != NULL
        && position[0] != '#');
    position = strchr(position, ' ') + 1) {

        // Consume multiple spaces, if needed.
        if (position[0] == ' ' || position[0] == '\n' ||
            position[0] == '\v' || position[0] == '\r') {
                continue;
        };

        // Parse the feature-value pair.
        int id = atoi(position);
        position = strchr(position, ':') + 1;
        float value = atof(position);
        pushPair(id, value);
    }

    // Parse comment, if any.
    position = strchr(in_string, '#');
    if (position != NULL) {
        m_comment = string(position + 1);
    }
}

//////////////////////////////

double SparseInstance::getAttributeValue(int) const
{
    return 0.0;
}

double SparseInstance::getOutputAttributeValue(int) const
{
    return (double)getY();
}

void SparseInstance::addValue(double, int)
{
}


Instance* SparseInstance::clone() {
	// TODO implement this function
	return nullptr;
}
