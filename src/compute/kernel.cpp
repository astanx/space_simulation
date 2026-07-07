#include "compute/kernel.h"

#include "debug/logger.h"

// Constructor / Destructor
Kernel::Kernel(cl_program program)
{
  cl_int errNum;
  this->kernel = clCreateKernel(program, "hello_kernel", &errNum);

  if (errNum != CL_SUCCESS)
    Logger::logError("Kernel", "Failed to create OpenCL kernel");
}

Kernel::~Kernel()
{
  clReleaseKernel(this->kernel);
}

// Public functions
void Kernel::setArg(cl_uint index, size_t size, const void *value)
{
  cl_int errNum = clSetKernelArg(this->kernel, index, size, value);

  if (errNum != CL_SUCCESS)
    Logger::logError("Kernel", "Failed to set OpenCL kernel argument for index " + std::to_string(index));
}