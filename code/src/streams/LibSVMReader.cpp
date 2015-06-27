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
// LibSVMReader.cpp
//
// Author: D. Sculley, December 2008
// dsculley@google.com or dsculley@cs.tufts.edu
//
// Implementation of LibSVMReader.h

#include <assert.h>
#include "LibSVMReader.h"

REGISTER_CLASS(LibSVMReader)
REGISTER_COMMAND_LINE_PARAMETER(
		LibSVMReader,
		"{\"type\":\"Reader\","
		"\"name\":\"LibSVMReader\","
		"\"parameter\":{"
		"\"-b\":\"UseBiasTerm\","
		"\"-s\":\"BufferSize\""
		"}}"
		"");

//----------------------------------------------------------------//
//------------------ SfDataSet Public Methods --------------------//
//----------------------------------------------------------------//

//LibSVMReader::LibSVMReader(bool use_bias_term)
//:m_use_bias_term(use_bias_term), m_buffersize_mb(40), m_file_buffer(NULL), m_pFileStream(NULL)
//{
//}
//
//LibSVMReader::LibSVMReader(int buffer_mb, bool use_bias_term)
//:m_use_bias_term(use_bias_term), m_buffersize_mb(buffer_mb), m_file_buffer(NULL), m_pFileStream(NULL)
//{
//}

LibSVMReader::LibSVMReader()
{
	m_use_bias_term = false;
	m_buffersize_mb = 40;
	m_file_buffer = NULL;
	m_pFileStream = NULL;
}


LibSVMReader::~LibSVMReader()
{
    if (NULL != m_file_buffer)
    {
        delete[] m_file_buffer;
    }
    
    if (NULL != m_pFileStream)
    {
        if (m_pFileStream->is_open())
        {
            m_pFileStream->close();
        }

        delete m_pFileStream;
    }
}

void LibSVMReader::doSetParams() {
	m_use_bias_term = getParam("UseBiasTerm", false);
	m_buffersize_mb = getParam("BufferSize", 40);
}

int LibSVMReader::input(string& line_string) 
{
    this->mHasNextInstance = false;
    this->mNextInstance = NULL;

    if (getline(*m_pFileStream, line_string)) 
    {
        this->mNextInstance = new SparseInstance(line_string.c_str(), m_use_bias_term);
        this->mHasNextInstance = true;
    }

    return 0;
}

bool LibSVMReader::setFile(const string& file_name)
{
    long int buffer_size = m_buffersize_mb * 1024 * 1024;
    m_file_buffer = new char[buffer_size];
    m_pFileStream = new std::ifstream(file_name.c_str(), std::ifstream::in);
    m_pFileStream->rdbuf()->pubsetbuf(m_file_buffer, buffer_size); 

    if (!m_pFileStream->is_open()) 
    {
        return false;
    }

    return true;
}

bool LibSVMReader::hasNextInstance()
{
    string line_string = "";
    input(line_string);
    return this->mHasNextInstance;
}
