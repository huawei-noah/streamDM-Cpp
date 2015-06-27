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
// sf-weight-vector.h
//
// Author: D. Sculley
// dsculley@google.com or dsculley@cs.tufts.edu
//
// A simple weight vector class used for training and classification
// with SfSaprseVector instances.  The dimensionality d of the data
// set must be specified at construction time, as the internal
// data is kept in a contiguous array for data concurrency.
//
// Note that the weight vector is composed of feature value pairs,
// and a scaling parameter scale_.  This scaling parameter allows for
// fast L2-norm regularization.
//
// The squared_norm_ member maintains the squared norm on all updates.

#ifndef SPARSEESTIMATOR_H__
#define SPARSEESTIMATOR_H__

#include "SparseInstance.h"

using std::string;

class SparseEstimator 
{
public:
    // Construct a weight vector of dimenson d, with all weights initialized to
    // zero, scale_ set to 1, and squared_norm_ set to 0.
    SparseEstimator(int dimensionality);

    // Constructs a weight vector from a string, which is identical in format
    // to that produced by the AsString() member method.
    SparseEstimator(const string& weight_vector_string);

    // Simple copy constructor, needed to allocate a new array of weights.
    SparseEstimator(const SparseEstimator& weight_vector);

    // Frees the array of weights.
    virtual ~SparseEstimator();

    // Re-scales weight vector to scale of 1, and then outputs each weight in
    // order, space separated.
    string asString();

    // Computes inner product of <x_scale * x, w>
    virtual float innerProduct(const Instance& x,
        float x_scale = 1.0) const;

    // Computes inner product of <x_scale * (a - b), w>
    float innerProductOnDifference(const Instance& a,
        const Instance& b,
        float x_scale = 1.0) const;

    // w += x_scale * x
    virtual void addVector(const Instance& instance, float x_scale);

    // w *= scaling_factor
    void scaleBy(double scaling_factor);

    // Returns value of element w_index, taking internal scaling into account.
    float valueOf(int index) const;

    // Project this vector into the L1 ball of radius lambda.
    void projectToL1Ball(float lambda);

    // Project this vector into the L1 ball of radius at most lambda, plus or
    // minus epsilon / 2.
    void projectToL1Ball(float lambda, float epsilon);

    // Getters.
    double getSquaredNorm() const { return m_squared_norm; }
    int getDimensions() const { return m_dimensions; }

protected:
    void scaleToOne();

    float* m_weights;
    double m_scale;
    double m_squared_norm;
    int m_dimensions;

private:
    // Disallowed.
    SparseEstimator();
};

#endif //SPARSEESTIMATOR_H__
