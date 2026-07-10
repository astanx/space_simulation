#pragma once

#include <OpenCL/cl.h>

#include <string>

class Kernel
{
protected:
  cl_kernel kernel;

public:
  Kernel(const std::string &name, cl_program program);
  ~Kernel();

  void setArg(cl_uint index, size_t size, const void *value);
};