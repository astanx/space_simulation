#include "compute/program.h"

#include "compute/context.h"

#include "debug/logger.h"

#include <fstream>
#include <sstream>

// Private functions
std::string Program::loadProgramSrc(const std::string filePath, bool isInclude)
{
  std::string temp = "";
  std::string src = "";

  std::ifstream inFile;

  inFile.open(filePath);

  if (inFile.is_open())
  {
    while (std::getline(inFile, temp))
    {
      if (temp.find("#include") == 0)
      {
        long start = temp.find("\"") + 1;
        long end = temp.find("\"", start);
        std::string includePath = temp.substr(start, end - start);
        src += this->loadProgramSrc(("assets/kernels/" + includePath).c_str(), true) + "\n";
      }
      else
        src += temp + "\n";
    }
  }
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

  this->program = clCreateProgramWithSource(context.get(), 1, &source, NULL, &errNum);

  if (errNum != CL_SUCCESS)
    Logger::logError("Program", "Failed to create OpenCL program");

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

  cl_int errNum = clBuildProgram(this->program, 1, &device, options.c_str(), nullptr, nullptr);

  if (errNum != CL_SUCCESS)
  {
    size_t logSize = 0;
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);

    std::vector<char> log(logSize);

    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logSize, log.data(), nullptr);

    Logger::logError("Program", "Program build failed: " + std::string(log.begin(), log.end()));
  }
}
