#pragma once

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include <string>

class Kernel
{
protected:
  cl_kernel kernel;

public:
  Kernel(const std::string &name, cl_program program);
  ~Kernel();

  void setArg(cl_uint index, size_t size, const void *value);
  void setArg(cl_uint index, cl_mem buffer);

  cl_kernel get() { return this->kernel; };
};