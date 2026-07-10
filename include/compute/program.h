#pragma once

#include <OpenCL/cl.h>
#include <string>

class Context;

class Program
{
protected:
  cl_program program;

  std::string loadProgramSrc(const std::string filePath, bool isInclude = false);

public:
  Program(const std::string filePath, Context& context);
  ~Program();

  void build(Context& context);
  cl_program get() { return this->program; };
};