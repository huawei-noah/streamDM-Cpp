//==========================================================================//
// Copyright 2009 Google Inc.                                               //
//                                                                          // 
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//      http://www.apache.org/licenses/LICENSE-2.0                          //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
//==========================================================================//
//
// sf-weight-vector.cc
//
// Author: D. Sculley
// dsculley@google.com or dsculley@cs.tufts.edu
//
// Implementation of sf-weight-vector.h

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

#include "SparseEstimator.h"

//----------------------------------------------------------------//
//---------------- SfWeightVector Public Methods ----------------//
//----------------------------------------------------------------//

SparseEstimator::SparseEstimator(int dimensionality) 
  : m_scale(1.0),
    m_squared_norm(0.0),
    m_dimensions(dimensionality) 
{
  if (m_dimensions <= 0) 
  {
    std::cerr << "Illegal dimensionality of weight vector less than 1."
	      << std::endl
	      << "dimensions_: " << m_dimensions << std::endl;
    exit(1);
  }

  m_weights = new float[m_dimensions];
  if (m_weights == NULL) 
  {
    std::cerr << "Not enough memory for weight vector of dimension: " 
	      <<  m_dimensions << std::endl;
    exit(1);
  }

  for (int i = 0; i < m_dimensions; ++i) 
  {
    m_weights[i] = 0;
  }
}

SparseEstimator::SparseEstimator(const string& weight_vector_string) 
  : m_scale(1.0),
    m_squared_norm(0.0),
    m_dimensions(0) 
{
  // Count dimensions in string.
  std::stringstream count_stream(weight_vector_string);
  float weight;
  while (count_stream >> weight) { 
    ++m_dimensions;
  }
    
  // Allocate weights_.
  m_weights = new float[m_dimensions];
  if (m_weights == NULL) 
  {
    std::cerr << "Not enough memory for weight vector of dimension: " 
	      <<  m_dimensions << std::endl;
    exit(1);
  }
  
  // Fill weights_ from weights in string.
  std::stringstream weight_stream(weight_vector_string);
  for (int i = 0; i < m_dimensions; ++i) {
    weight_stream >> m_weights[i];
    m_squared_norm += m_weights[i] * m_weights[i];
  }
}

SparseEstimator::SparseEstimator(const SparseEstimator& weight_vector) 
{
  m_scale = weight_vector.m_scale;
  m_squared_norm = weight_vector.m_squared_norm;
  m_dimensions = weight_vector.m_dimensions;

  m_weights = new float[m_dimensions];
  if (m_weights == NULL) {
    std::cerr << "Not enough memory for weight vector of dimension: " 
	      <<  m_dimensions << std::endl;
    exit(1);
  }

  for (int i = 0; i < m_dimensions; ++i) {
    m_weights[i] = weight_vector.m_weights[i];
  }
}

SparseEstimator::~SparseEstimator() {
  delete[] m_weights;
}

string SparseEstimator::asString() {
  scaleToOne();
  std::stringstream out_string_stream;
  for (int i = 0; i < m_dimensions; ++i) {
    out_string_stream << m_weights[i];
    if (i < (m_dimensions - 1)) {
      out_string_stream << " ";
    }
  }
  return out_string_stream.str();
}

float SparseEstimator::innerProduct(const Instance& instance,
				    float x_scale) const 
{
  float inner_product = 0.0;

  for (int i = 0; i < instance.getNumberInputAttributes(); ++i) 
  {
    inner_product += m_weights[instance.getInputAttributeID(i)] * (float)instance.getInputAttributeValue(i);
  }

  inner_product *= x_scale;
  inner_product *= m_scale;

  return inner_product;
}

float SparseEstimator::innerProductOnDifference(const Instance& a,
					       const Instance& b,
					       float x_scale) const {
  //   <x_scale * (a - b), w>
  // = <x_scale * a - x_scale * b, w>
  // = <x_scale * a, w> + <-1.0 * x_scale * b, w>

  float inner_product = 0.0;
  inner_product += innerProduct(a, x_scale);
  inner_product += innerProduct(b, -1.0 * x_scale);
  return inner_product;
}

void SparseEstimator::addVector(const Instance& instance, float x_scale) 
{
    if (instance.getInputAttributeID(instance.getNumberInputAttributes() - 1) > m_dimensions) 
    {
    std::cerr << "Feature " << instance.getInputAttributeID(instance.getNumberInputAttributes() - 1) 
	      << " exceeds dimensionality of weight vector: " 
	      << m_dimensions << std::endl;
    std::cerr << instance.toJson().toStyledString() << std::endl;
    exit(1);
  }

  float inner_product = 0.0;
  for (int i = 0; i < instance.getNumberInputAttributes(); ++i) 
  {
      float this_x_value = instance.getInputAttributeValue(i) * x_scale;
    int this_x_feature = instance.getInputAttributeID(i);
    inner_product += m_weights[this_x_feature] * this_x_value;
    m_weights[this_x_feature] += this_x_value / m_scale;
  }

  m_squared_norm += instance.getSquaredNorm() * x_scale * x_scale +
    (2.0 * m_scale * inner_product); 
}

void SparseEstimator::scaleBy(double scaling_factor) {
  // Take care of any numerical difficulties.
  if (m_scale < 0.00000000001) scaleToOne();

  // Do the scaling.
  m_squared_norm *= (scaling_factor * scaling_factor);
  if (scaling_factor > 0.0) {
    m_scale *= scaling_factor;
  } else {
    std::cerr << "Error: scaling weight vector by non-positive value!\n " 
	      << "This can cause numerical errors in PEGASOS projection.\n "
	      << "This is likely due to too large a value of eta * lambda.\n "
	      << std::endl;
    exit(1);
  }
}

float SparseEstimator::valueOf(int index) const {
  if (index < 0) {
    std::cerr << "Illegal index " << index << " in ValueOf. " << std::endl;
    exit(1);
  }
  if (index >= m_dimensions) {
    return 0;
  }
  return m_weights[index] * m_scale;
}

void SparseEstimator::projectToL1Ball(float lambda, float epsilon) {
  // Re-scale lambda.
  lambda = lambda / m_scale;

  // Bail out early if possible.
  float current_l1 = 0.0;
  float max_value = 0.0;
  vector<float> non_zeros;
  for (int i = 0; i < m_dimensions; ++i) {
    if (m_weights[i] != 0.0) {
      non_zeros.push_back(fabsf(m_weights[i]));
    } else {
      continue;
    }
    current_l1 += fabsf(m_weights[i]);
    if (fabs(m_weights[i]) > max_value) {
      max_value = fabs(m_weights[i]);
    }
  }
  if (current_l1 <= (1.0 + epsilon) * lambda) return;

  float min = 0;
  float max = max_value;
  float theta = 0.0;
  while (current_l1 >  (1.0 + epsilon) * lambda ||
	 current_l1 < lambda) {
    theta = (max + min) / 2.0;
    current_l1 = 0.0;
    for (unsigned int i = 0; i < non_zeros.size(); ++i) {
      current_l1 += std::max(0.0f, non_zeros[i] - theta);
    }
    if (current_l1 <= lambda) {
      max = theta;
    } else {
      min = theta;
    }
  }

  for (int i = 0; i < m_dimensions; ++i) {
    if (m_weights[i] > 0) m_weights[i] = std::max(0.0f, m_weights[i] - theta);
    if (m_weights[i] < 0) m_weights[i] = std::min(0.0f, m_weights[i] + theta);
  } 
}


void SparseEstimator::projectToL1Ball(float lambda) {
  // Bail out early if possible.
  float current_l1 = 0.0;
  for (int i = 0; i < m_dimensions; ++i) {
    if (fabsf(valueOf(i)) > 0) current_l1 += fabsf(valueOf(i));
  }
  if (current_l1 < lambda) return;

  vector<int> workspace_a;
  vector<int> workspace_b;
  vector<int> workspace_c;
  vector<int>* U = &workspace_a;
  vector<int>* L = &workspace_b;
  vector<int>* G = &workspace_c;
  vector<int>* temp;
  // Populate U with all non-zero elements in weight vector.
  for (int i = 0; i < m_dimensions; ++i) {
    if (fabsf(valueOf(i)) > 0) {
      U->push_back(i);
      current_l1 += fabsf(valueOf(i));
    }
  }

  // Find the value of theta.
  double partial_pivot = 0;
  double partial_sum = 0;
  while (U->size() > 0) {
    G->clear();
    L->clear();
    int k = (*U)[static_cast<int>(rand() % U->size())];
    float pivot_k = fabsf(valueOf(k));
    float partial_sum_delta = fabsf(valueOf(k));
    float partial_pivot_delta = 1.0;
    // Partition U using pivot_k.
    for (unsigned int i = 0; i < U->size(); ++i) {
      float w_i = fabsf(valueOf((*U)[i]));
      if (w_i >= pivot_k) {
	if ((*U)[i] != k) {
	  partial_sum_delta += w_i;
	  partial_pivot_delta += 1.0;
	  G->push_back((*U)[i]);
	}
      } else {
	L->push_back((*U)[i]);
      }
    }
    if ((partial_sum + partial_sum_delta) -
	pivot_k * (partial_pivot + partial_pivot_delta) < lambda) {
      partial_sum += partial_sum_delta;
      partial_pivot += partial_pivot_delta;
      temp = U;
      U = L;
      L = temp;
    } else {
      temp = U;
      U = G;
      G = temp;
    }
  }

  // Perform the projection.
  float theta = (partial_sum - lambda) / partial_pivot;  
  m_squared_norm = 0.0;
  for (int i = 0; i < m_dimensions; ++i) {
    if (valueOf(i) == 0.0) continue;
    int sign = (valueOf(i) > 0) ? 1 : -1;
    m_weights[i] = sign * std::max((sign * valueOf(i) - theta), 0.0f); 
    m_squared_norm += m_weights[i] * m_weights[i];
  }
  m_scale = 1.0;
}


//-----------------------------------------------------------------//
//---------------- SfWeightVector Private Methods ----------------//
//-----------------------------------------------------------------//

void SparseEstimator::scaleToOne() {
  for (int i = 0; i < m_dimensions; ++i) {
    m_weights[i] *= m_scale;
  }
  m_scale = 1.0;
}
