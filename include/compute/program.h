#pragma once

#include <OpenCL/cl.h>
#include <string>

class Program
{
protected:
  cl_program program;

public:
  Program(std::string fileName, cl_context context);
  ~Program();

  void build(cl_device_id device);
};