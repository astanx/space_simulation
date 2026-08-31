#pragma once

#include <GL/glew.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#include <CL/cl_gl.h>
#endif

class CLBuffer
{
protected:
  cl_mem buffer;

public:
  CLBuffer() = default;
  CLBuffer(cl_context context, cl_mem_flags flag, size_t size, void *data);
  CLBuffer(cl_context context, cl_mem_flags flag, GLuint buffer);
  ~CLBuffer();

  void init(cl_context context, cl_mem_flags flag, size_t size, void *data);
  void init(cl_context context, cl_mem_flags flag, GLuint buffer);

  cl_mem get() const { return this->buffer; };
};