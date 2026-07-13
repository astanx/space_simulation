#include "compute/kernel.h"

#include "debug/logger.h"

// Constructor / Destructor
Kernel::Kernel(const std::string &name, cl_program program)
{
  cl_int errNum;
  CL_CREATE(this->kernel = clCreateKernel(program, name.c_str(), &errNum), errNum);
}

Kernel::~Kernel()
{
  clReleaseKernel(this->kernel);
}

// Public functions
void Kernel::setArg(cl_uint index, size_t size, const void *value)
{
  CL_CALL(clSetKernelArg(this->kernel, index, size, value));
}

void Kernel::setArg(cl_uint index, cl_mem buffer)
{
  CL_CALL(clSetKernelArg(this->kernel, index, sizeof(cl_mem), &buffer));
}
