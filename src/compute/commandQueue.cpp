#include "compute/commandQueue.h"

#include "debug/logger.h"

#include <OpenCL/cl_gl.h>

// Constructor / Destructor
CommandQueue::CommandQueue(cl_context context, cl_device_id device)
{
  cl_int errNum;
  this->queue = clCreateCommandQueue(context, device, 0, &errNum);

  if (errNum != CL_SUCCESS)
    Logger::logError("CommandQueue", "Failed to create command queue");
}

CommandQueue::~CommandQueue()
{
  clReleaseCommandQueue(this->queue);
}

// Public functions
void CommandQueue::enqueueWriteBuffer(cl_mem buffer, cl_bool blockWrite, size_t offset, size_t size, const void *data)
{
  cl_int errNum = clEnqueueWriteBuffer(this->queue, buffer, blockWrite, offset, size, data, 0, nullptr, nullptr);
  if (errNum != CL_SUCCESS)
    Logger::logError("CommandQueue", "Failed to enqueue write buffer");
}
void CommandQueue::enqueueReadBuffer(cl_mem buffer, cl_bool blockRead, size_t offset, size_t size, void *store)
{
  cl_int errNum = clEnqueueReadBuffer(this->queue, buffer, blockRead, offset, size, store, 0, nullptr, nullptr);
  if (errNum != CL_SUCCESS)
    Logger::logError("CommandQueue", "Failed to enqueue read buffer");
}
void CommandQueue::enqueueAcquireGLBuffer(const cl_mem buffer)
{
  cl_int errNum = clEnqueueAcquireGLObjects(this->queue, 1, &buffer, 0, nullptr, nullptr);
  if (errNum != CL_SUCCESS)
    Logger::logError("CommandQueue", "Failed to enqueue acquire OpenGL buffer");
}
void CommandQueue::enqueueReleaseGLBuffer(const cl_mem buffer)
{
  cl_int errNum = clEnqueueReleaseGLObjects(this->queue, 1, &buffer, 0, nullptr, nullptr);
  if (errNum != CL_SUCCESS)
    Logger::logError("CommandQueue", "Failed to enqueue release OpenGL buffer");
}
void CommandQueue::enqueueNDKernelBuffer(cl_kernel kernel, cl_uint dimensions, const size_t globalWorkSize, const size_t localWorkSize)
{
  cl_int errNum = clEnqueueNDRangeKernel(this->queue, kernel, dimensions, nullptr, &globalWorkSize, &localWorkSize, 0, nullptr, nullptr);
  if (errNum != CL_SUCCESS)
    Logger::logError("CommandQueue", "Failed to enqueue ND kernel buffer");
}

void CommandQueue::finish()
{
  clFinish(this->queue);
}