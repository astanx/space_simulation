#include "compute/program.h"

#include "compute/context.h"

#include "debug/logger.h"

#include <fstream>
#include <sstream>
#include <iostream>

// Private functions
std::string Program::loadProgramSrc(const std::string filePath, bool isInclude)
{
  std::string temp = "";
  std::string src = "";

  std::ifstream inFile;

  inFile.open(filePath);

  if (inFile.is_open())
    while (std::getline(inFile, temp))
      src += temp + "\n";
  else
    Logger::logError("Program", "Could not open program file: " + filePath);

  inFile.close();

  return src;
}

// Constructor / Destructor
Program::Program(const std::string filePath, Context &context)
{
  std::string src = this->loadProgramSrc(filePath);
  const char *source = src.c_str();

  cl_int errNum;

  CL_CREATE(this->program = clCreateProgramWithSource(context.get(), 1, &source, NULL, &errNum), errNum);

  this->build(context);
}

Program::~Program()
{
  clReleaseProgram(this->program);
}

// Public functiosn
void Program::build(Context &context)
{
  cl_device_id device = context.getDevice();

  std::string options;

  if (context.getSupportsDouble())
    options = "-DUSE_DOUBLE";

  options += " -I include";
  options += " -I assets/kernels";

  CL_CALL(clBuildProgram(this->program, 1, &device, options.c_str(), nullptr, nullptr));

  size_t logSize = 0;

  clGetProgramBuildInfo(this->program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);

  if (logSize > 1)
  {
    std::vector<char> log(logSize);

    clGetProgramBuildInfo(this->program, device, CL_PROGRAM_BUILD_LOG, logSize, log.data(), nullptr);

    Logger::logInfo("Program", std::string(log.data()));
  }
}
