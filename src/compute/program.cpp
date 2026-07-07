#include "compute/program.h"

#include "debug/logger.h"

#include <fstream>
#include <sstream>

// Constructor / Destructor
Program::Program(std::string fileName, cl_context context)
{
  std::ifstream kernelFile(fileName, std::ios::in);
  if (!kernelFile.is_open())
    Logger::logError("Program", "Failed to open file for reading: " + fileName);

  std::ostringstream oss;
  oss << kernelFile.rdbuf();
  std::string srcStdStr = oss.str();
  const char *srcStr = srcStdStr.c_str();

  cl_int errNum;

  this->program = clCreateProgramWithSource(context, 1, &srcStr, NULL, &errNum);

  if (errNum != CL_SUCCESS)
    Logger::logError("Program", "Failed to create OpenCL program");
}

Program::~Program()
{
  clReleaseProgram(this->program);
}

// Public functiosn
void Program::build(cl_device_id device)
{
  clBuildProgram(this->program, 1, &device, nullptr, nullptr, nullptr);
}
