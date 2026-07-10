#include "compute/commandQueue.h"

#include "debug/logger.h"

#include <OpenCL/cl_gl.h>

// Constructor / Destructor
CommandQueue::CommandQueue(cl_context context, cl_device_id device)
{
  cl_int errNum;
  CL_CREATE(this->queue = clCreateCommandQueue(context, device, 0, &errNum), errNum);
}

CommandQueue::~CommandQueue()
{
  clReleaseCommandQueue(this->queue);
}

// Public functions
void CommandQueue::enqueueWriteBuffer(cl_mem buffer, cl_bool blockWrite, size_t offset, size_t size, const void *data)
{
  CL_CALL(clEnqueueWriteBuffer(this->queue, buffer, blockWrite, offset, size, data, 0, nullptr, nullptr));
}
void CommandQueue::enqueueReadBuffer(cl_mem buffer, cl_bool blockRead, size_t offset, size_t size, void *store)
{
  CL_CALL(clEnqueueReadBuffer(this->queue, buffer, blockRead, offset, size, store, 0, nullptr, nullptr));
}
void CommandQueue::enqueueAcquireGLBuffer(const cl_mem buffer)
{
  CL_CALL(clEnqueueAcquireGLObjects(this->queue, 1, &buffer, 0, nullptr, nullptr));
}
void CommandQueue::enqueueReleaseGLBuffer(const cl_mem buffer)
{
  CL_CALL(clEnqueueReleaseGLObjects(this->queue, 1, &buffer, 0, nullptr, nullptr));
}
void CommandQueue::enqueueNDKernelBuffer(cl_kernel kernel, cl_uint dimensions, const size_t globalWorkSize, const size_t localWorkSize)
{
  CL_CALL(clEnqueueNDRangeKernel(this->queue, kernel, dimensions, nullptr, &globalWorkSize, &localWorkSize, 0, nullptr, nullptr));
}

void CommandQueue::finish()
{
  clFinish(this->queue);
}