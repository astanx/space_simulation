#include "compute/commandQueue.h"

#include "debug/logger.h"

#include <OpenCL/cl_gl.h>
#include <iostream>

// Constructor / Destructor
CommandQueue::CommandQueue(cl_context context, cl_device_id device)
{
  this->init(context, device);
}

CommandQueue::~CommandQueue()
{
  clReleaseCommandQueue(this->queue);
}

// Public functions
void CommandQueue::init(cl_context context, cl_device_id device)
{
  cl_int errNum;
  CL_CREATE(this->queue = clCreateCommandQueue(context, device, 0, &errNum), errNum);
}

void CommandQueue::enqueueFillBuffer(cl_mem buffer, const void *pattern, size_t pattern_size, size_t size, cl_event *event)
{
  CL_CALL(clEnqueueFillBuffer(this->queue, buffer, pattern, pattern_size, 0, size, 0, nullptr, event));
}
void CommandQueue::enqueueWriteBuffer(cl_mem buffer, cl_bool blockWrite, size_t offset, size_t size, const void *data)
{
  CL_CALL(clEnqueueWriteBuffer(this->queue, buffer, blockWrite, offset, size, data, 0, nullptr, nullptr));
}
void CommandQueue::enqueueReadBuffer(cl_mem buffer, cl_bool blockRead, size_t offset, size_t size, void *store, cl_event *event)
{
  CL_CALL(clEnqueueReadBuffer(this->queue, buffer, blockRead, offset, size, store, 0, nullptr, event));
}
void CommandQueue::enqueueAcquireGLBuffer(const cl_mem buffer)
{
  CL_CALL(clEnqueueAcquireGLObjects(this->queue, 1, &buffer, 0, nullptr, nullptr));
}
void CommandQueue::enqueueReleaseGLBuffer(const cl_mem buffer)
{
  CL_CALL(clEnqueueReleaseGLObjects(this->queue, 1, &buffer, 0, nullptr, nullptr));
}
void CommandQueue::enqueueNDKernelBuffer(cl_kernel kernel, cl_uint dimensions, const size_t *offset, const size_t *globalWorkSize, const size_t *localWorkSize)
{
  if (globalWorkSize && *globalWorkSize == 0)
    Logger::logWarning("Command Queue", "Kernel with 0 global work size enqueued, skipping");
  else
    CL_CALL(clEnqueueNDRangeKernel(this->queue, kernel, dimensions, offset, globalWorkSize, localWorkSize, 0, nullptr, nullptr));
}
void CommandQueue::enqueueNDKernelBuffer(cl_kernel kernel, cl_uint dimensions, const size_t *offset, const size_t *globalWorkSize, const size_t *localWorkSize, cl_event *event)
{
  if (globalWorkSize && *globalWorkSize == 0)
    Logger::logWarning("Command Queue", "Kernel with 0 global work size enqueued, skipping");
  else
    CL_CALL(clEnqueueNDRangeKernel(this->queue, kernel, dimensions, offset, globalWorkSize, localWorkSize, 0, nullptr, event));
}
void CommandQueue::enqueueNDKernelBuffer(cl_kernel kernel, cl_uint dimensions, const size_t *offset, const size_t *globalWorkSize)
{
  if (globalWorkSize && *globalWorkSize == 0)
    Logger::logWarning("Command Queue", "Kernel with 0 global work size enqueued, skipping");
  else
    CL_CALL(clEnqueueNDRangeKernel(this->queue, kernel, dimensions, offset, globalWorkSize, nullptr, 0, nullptr, nullptr));
}
void CommandQueue::enqueueNDKernelBuffer(cl_kernel kernel, cl_uint dimensions, const size_t *offset, const size_t *globalWorkSize, cl_event *event)
{
  if (globalWorkSize && *globalWorkSize == 0)
    Logger::logWarning("Command Queue", "Kernel with 0 global work size enqueued, skipping");
  else
    CL_CALL(clEnqueueNDRangeKernel(this->queue, kernel, dimensions, offset, globalWorkSize, nullptr, 0, nullptr, event));
}

void CommandQueue::finish()
{
  clFinish(this->queue);
}