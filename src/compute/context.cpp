#include "compute/context.h"

#include "debug/logger.h"

#include <OpenCL/gcl.h>
#include <OpenGL/CGLCurrent.h>

#include <vector>

// Constructor / Destructor
Context::Context()
{
  cl_uint numPlatforms;
  clGetPlatformIDs(0, NULL, &numPlatforms);

  std::vector<cl_platform_id> platforms(numPlatforms);
  clGetPlatformIDs(numPlatforms, platforms.data(), NULL);

  cl_platform_id bestPlatform = NULL;

  for (cl_uint i = 0; i < numPlatforms; i++)
  {
    cl_uint numDevices;
    clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);

    std::vector<cl_device_id> devices(numDevices);
    clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, numDevices, devices.data(), NULL);

    cl_uint bestComputeUnits = 0;

    for (cl_uint j = 0; j < numDevices; j++)
    {
      cl_uint computeUnits;
      clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &computeUnits, NULL);

      if (computeUnits > bestComputeUnits)
      {
        bestComputeUnits = computeUnits;
        this->device = devices[j];
        bestPlatform = platforms[i];
      }
    }
  }

  char extensions[4096];
  clGetDeviceInfo(this->device, CL_DEVICE_EXTENSIONS, sizeof(extensions), extensions, nullptr);

  this->supportsDouble = strstr(extensions, "cl_khr_fp64") != nullptr;

  if (this->supportsDouble)
    Logger::logInfo("Context", "Device supports OpenCL double precision");
  else
    Logger::logWarning("Context", "Device does NOT support OpenCL double precision");

  cl_context_properties contextProperties[7];
  int i = 0;

  contextProperties[i++] = CL_CONTEXT_PLATFORM;
  contextProperties[i++] = (cl_context_properties)bestPlatform;

#ifdef _WIN32
  contextProperties[i++] = CL_GL_CONTEXT_KHR;
  contextProperties[i++] = (cl_context_properties)wglGetCurrentContext();
  contextProperties[i++] = CL_WGL_HDC_KHR;
  contextProperties[i++] = (cl_context_properties)wglGetCurrentDC();
#endif

#ifdef __linux__
  contextProperties[i++] = CL_GL_CONTEXT_KHR;
  contextProperties[i++] = (cl_context_properties)glXGetCurrentContext();
  contextProperties[i++] = CL_GLX_DISPLAY_KHR;
  contextProperties[i++] = (cl_context_properties)glXGetCurrentDisplay();
#endif
#ifdef __APPLE__
  contextProperties[i++] = CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE;
  contextProperties[i++] = (cl_context_properties)CGLGetShareGroup(CGLGetCurrentContext());
#endif

  contextProperties[i] = 0;

  cl_int errNum;
  CL_CREATE(this->context = clCreateContext(contextProperties, 1, &this->device, nullptr, nullptr, &errNum), errNum);
}

Context::~Context()
{
  clReleaseContext(this->context);
}