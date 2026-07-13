#include "compute/clBuffer.h"

#include "debug/logger.h"

#include <OpenCL/gcl.h>

// Constructor / Destructor
CLBuffer::CLBuffer(cl_context context, cl_mem_flags flag, size_t size, void *data)
{
  this->init(context, flag, size, data);
}

CLBuffer::CLBuffer(cl_context context, cl_mem_flags flag, GLuint buffer)
{
  this->init(context, flag, buffer);
}

CLBuffer::~CLBuffer()
{
  clReleaseMemObject(this->buffer);
}

// Public functions
void CLBuffer::init(cl_context context, cl_mem_flags flag, size_t size, void *data)
{
  cl_int errNum;
  CL_CREATE(this->buffer = clCreateBuffer(context, flag, size, data, &errNum), errNum);
}

void CLBuffer::init(cl_context context, cl_mem_flags flag, GLuint buffer)
{
  cl_int errNum;
  CL_CREATE(this->buffer = clCreateFromGLBuffer(context, flag, buffer, &errNum), errNum);
}