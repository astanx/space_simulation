#include "compute/program.h"

#include "debug/logger.h"

#include <fstream>
#include <sstream>

// Private functions
std::string Program::loadProgramSrc(const std::string fileName, bool isInclude)
{
  std::string temp = "";
  std::string src = "";

  std::ifstream inFile;

  inFile.open(fileName);

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
    Logger::logError("Program", "Could not open program file: " + fileName);

  inFile.close();

  return src;
}

// Constructor / Destructor
Program::Program(std::string fileName, cl_context context)
{
  std::string src = this->loadProgramSrc(fileName);
  const char *source = src.c_str();

  cl_int errNum;

  this->program = clCreateProgramWithSource(context, 1, &source, NULL, &errNum);

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
