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
// sf-data-set.h
//
// Author: D. Sculley, December 2008
// dsculley@google.com or dsculley@cs.tufts.edu
//
// A data set object based on SfSparseVector, as defined in
// sf-sparse-vector.h.  Methods are provided for reading
// a data set into memory from a file, and accessing individual
// vectors within the data set.

#ifndef LIBSVMREADER_H__
#define LIBSVMREADER_H__

#include "../Common.h"

class LibSVMReader : public Reader 
{
public:
//    // Empty data set.
//    LibSVMReader(bool use_bias_term);
//
//    // Construct and fill a SfDataSet with data from the given file.
//    // Use buffer_mb megabytes for the buffer.
//    LibSVMReader(int buffer_mb, bool use_bias_term);

    LibSVMReader();

    virtual ~LibSVMReader();

public:
	virtual int input(string&);
	virtual bool setFile(const string&);
    virtual bool hasNextInstance();

private:
    // Member containing all vectors in data set.
    //vector<SfSparseVector> vectors_;
    // Should we add a bias term to each new vector in the data set?
    bool m_use_bias_term;
    int m_buffersize_mb;
    std::ifstream* m_pFileStream;
    char* m_file_buffer;

    void doSetParams();
};

#endif  // LIBSVMREADER_H__
