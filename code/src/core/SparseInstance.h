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
// sf-sparse-vector.h
//
// Author: D. Sculley, December 2008
// dsculley@google.com or dsculley@cs.tufts.edu
//
// A sparse vector for use with sofia-ml.  Vector elements are contained
// in an stl vector, stored in a struct containing (feature id, feature value)
// pairs.  Feature id's are assumed to be unique, sorted, and strictly positive.
// Sparse vector is assumed to be in svm-light format, with the distinction
// that the class label may be a float rather than an integer, and
// that there is an optional group id value.
//
// <label> <group id?> <feature>:<value> ... <feature><value> <#comment?>
//
// The group id is set by a <prefix>:<id number>, such as qid:3.  For
// our purposes, the actual prefix string is ignored.  The only requirement
// is that it begins with a letter in [a-zA-Z].
//
// The comment string is optional.  If it is needed, use a # character
// to begin the comment.  The remainder of the string is then treated
// as the comment.
//
// Note that features must be sorted in ascending order, by feature id.
// Also, feature id 0 is reserved for the bias term.

#ifndef SPARSEINSTANCE_H__
#define SPARSEINSTANCE_H__

#include <float.h>
#include <string>
#include <vector>
#include "../utils/json.h"

#include "Instance.h"

#define SF_UNDEFINED_VAL FLT_MAX

using std::string;
using std::vector;

// Each element of the SfSparseVector is represented as a FeatureValuePair.
// Bundling these as a struct improves memory locality.
struct FeatureValuePair {
    int id_;
    float value_;
};

class SparseInstance : public Instance
{
public:
    // Construct a new vector from a string.  Input format is svm-light format:
    // <label> <feature>:<value> ... <feature:value> # comment<\n>
    // No bias term is used.
    SparseInstance(const char* in_string);

    // Constructs a new vector from a string, as above, but also sets the bias
    // term to 1 iff use_bias_term is set to true.
    SparseInstance(const char* in_string, bool use_bias_term);

    // Construct a new vector that is the difference of two vectors, (a - b).
    // This is useful for ranking problems, etc.
    SparseInstance(const SparseInstance& a, const SparseInstance& b, float y);

public:
    //Values
    virtual double getAttributeValue(int) const;
    virtual double getOutputAttributeValue(int) const;
    virtual Json::Value toJson() const;
    virtual void addValue(double, int);

    // Returns a string-format representation of the vector, in svm-light format.
    //string AsString() const;

    // Methods for interacting with features
    inline int numFeatures() const { return m_features.size(); }
    virtual int getNumberInputAttributes() const { return m_features.size(); }
    //inline int FeatureAt(int i) const { return m_features[i].id_; }
    virtual int getInputAttributeID(int i) const { return m_features[i].id_; }
    virtual double getInputAttributeValue(int i) const { return (double)m_features[i].value_; }
    //inline float ValueAt(int i) const { return m_features[i].value_; }

    // Getters and setters.
    void setY(float new_y) { m_y = new_y; }
    void setA(float new_a) { m_a = new_a; }
    void setGroupId(const string& new_id) { m_group_id = new_id; }
    void setComment(const string& new_comment) { m_comment = new_comment; }
    float getY() const { return m_y; }
    float getA() const { return m_a; }
    virtual float getSquaredNorm() const { return m_squared_norm; }
    const string& getGroupId() const { return m_group_id; }
    const string& getComment() const { return m_comment; }

    // Adds a new (id, value) FeatureValuePair to the end of the vector, and
    // updates the internal squared_norm_ member.
    void pushPair (int id, float value);

    // Clear all feature values and the cached squared_norm_, leaving all
    // other information unchanged.
    void clearFeatures() { m_features.clear(); m_squared_norm = 0; }

    Instance* clone();

private:
    void addToSquaredNorm(float addend) { m_squared_norm += addend; }

    // Common initialization method shared by constructors, adding vector data
    // by parsing a string in SVM-light format.
    void init(const char* in_string);

    // Sets up the bias term, indexed by feature id 0.
    void setBias() { pushPair(0, 1); }

    // Sets up the bias term as null value, indexed by feature id 0.
    void noBias() { pushPair(0, 0); }

    // Exits if the input format of the file is incorrect.
    void dieFormat(const string& reason);

    // Members.
    // Typically, only non-zero valued features are stored.  This vector is assumed
    // to hold feature id, feature value pairs in order sorted by feature id.  The
    // special feature id 0 is always set to 1, encoding bias.
    vector<FeatureValuePair> m_features;

    // m_y is the class label.  We store this as a float, rather than an int,
    // so that this class may be used for regression problems, etc., if desired.
    float m_y;

    // m_a is the current alpha value in optimization.
    float m_a;

    // m_squared_norm = x1*x1 + ... + xN*xN
    float m_squared_norm;

    // Use this member when examples belong to distinct groups.  For instance,
    // in ranking problems examples are grouped by query id.  By default,
    // this is set to 0.
    string m_group_id;

    // m_comment can be any string-based comment.
    string m_comment;
};

#endif // SPARSEINSTANCE_H__
