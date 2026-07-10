#include "compute/clBuffer.h"

#include "debug/logger.h"

#include <OpenCL/gcl.h>

// Constructor / Destructor
CLBuffer::CLBuffer(cl_context context, cl_mem_flags flag, size_t size)
{
  cl_int errNum;
  CL_CREATE(this->buffer = clCreateBuffer(context, flag, size, nullptr, &errNum), errNum);
}

CLBuffer::CLBuffer(cl_context context, cl_mem_flags flag, GLuint buffer)
{
  cl_int errNum;
  CL_CREATE(this->buffer = clCreateFromGLBuffer(context, flag, buffer, &errNum), errNum);
}

CLBuffer::~CLBuffer()
{
  clReleaseMemObject(this->buffer);
}