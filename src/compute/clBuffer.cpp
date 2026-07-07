#include "compute/clBuffer.h"

#include "debug/logger.h"

#include <OpenCL/gcl.h>

// Constructor / Destructor
CLBuffer::CLBuffer(cl_context context, cl_mem_flags flag, size_t size)
{
  cl_int errNum;
  this->buffer = clCreateBuffer(context, flag, size, nullptr, &errNum);

  if (errNum != CL_SUCCESS)
    Logger::logError("CLBuffer", "Failed to create OpenCL buffer");
}

CLBuffer::CLBuffer(cl_context context, cl_mem_flags flag, GLuint buffer)
{
  cl_int errNum;
  this->buffer = clCreateFromGLBuffer(context, flag, buffer, &errNum);

  if (errNum != CL_SUCCESS)
    Logger::logError("CLBuffer", "Failed to create OpenCL buffer from OpenGL buffer");
}

CLBuffer::~CLBuffer()
{
  clReleaseMemObject(this->buffer);
}