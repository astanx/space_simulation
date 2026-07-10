#pragma once

#include <OpenCL/cl.h>

class Context
{
protected:
  cl_context context;
  cl_device_id device;

  bool supportsDouble;

public:
  Context();
  ~Context();

  cl_context get() { return this->context; };
  cl_device_id getDevice() { return this->device; };
  bool getSupportsDouble() { return this->supportsDouble; };
};