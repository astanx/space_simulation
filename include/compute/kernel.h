#pragma once

#include <OpenCL/cl.h>

class Kernel
{
protected:
  cl_kernel kernel;

public:
  Kernel(cl_program program);
  ~Kernel();

  void setArg(cl_uint index, size_t size, const void *value);
};