#pragma once

#include <OpenCL/cl.h>
#include <GL/glew.h>

class CLBuffer
{
protected:
  cl_mem buffer;

public:
  CLBuffer(cl_context context, cl_mem_flags flag, size_t size);
  CLBuffer(cl_context context, cl_mem_flags flag, GLuint buffer);
  ~CLBuffer();
};