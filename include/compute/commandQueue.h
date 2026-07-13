#pragma once

#include <OpenCL/cl.h>

class CommandQueue
{
protected:
  cl_command_queue queue;

public:
  CommandQueue() = default;
  CommandQueue(cl_context context, cl_device_id device);
  ~CommandQueue();

  void init(cl_context context, cl_device_id device);

  void enqueueWriteBuffer(cl_mem buffer, cl_bool blockWrite, size_t offset, size_t size, const void *data);
  void enqueueReadBuffer(cl_mem buffer, cl_bool blockRead, size_t offset, size_t size, void *store);
  void enqueueAcquireGLBuffer(const cl_mem buffer);
  void enqueueReleaseGLBuffer(const cl_mem buffer);
  void enqueueNDKernelBuffer(cl_kernel kernel, cl_uint dimensions, const size_t *offset, const size_t *globalWorkSize, const size_t *localWorkSize);
  void enqueueNDKernelBuffer(cl_kernel kernel, cl_uint dimensions, const size_t *offset, const size_t *globalWorkSize, const size_t *localWorkSize, cl_event *event);
  void enqueueNDKernelBuffer(cl_kernel kernel, cl_uint dimensions, const size_t *offset, const size_t *globalWorkSize);
  void enqueueNDKernelBuffer(cl_kernel kernel, cl_uint dimensions, const size_t *offset, const size_t *globalWorkSize, cl_event *event);

  void finish();
};