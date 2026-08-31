#pragma once

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#include <filesystem>
#include <string>
#include <vector>

class Context;

class Program
{
protected:
  cl_program program;

  std::string loadProgramSrc(const std::filesystem::path& filePath, std::vector<std::filesystem::path> includes = {});

public:
  Program(const std::string filePath, Context& context, std::vector<std::filesystem::path> includes = {});
  ~Program();

  void build(Context& context);
  cl_program get() { return this->program; };
};