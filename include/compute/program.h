#pragma once

#include <OpenCL/cl.h>
#include <filesystem>
#include <string>

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