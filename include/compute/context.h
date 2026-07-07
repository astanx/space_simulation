#pragma once

#include <OpenCL/cl.h>

class Context
{
protected:
  cl_context context;
  cl_device_id device;

public:
  Context();
  ~Context();
};