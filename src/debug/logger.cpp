#include "debug/logger.h"

#include "GL/glew.h"
#include <OpenCL/cl.h>

#include <iostream>
#include <unordered_set>
#include <mutex>
#include <map>

static std::map<GLenum, const std::string> GLErrors{
    {GL_INVALID_ENUM, "INVALID ENUM"},
    {GL_INVALID_VALUE, "INVALID VALUE"},
    {GL_INVALID_OPERATION, "INVALID OPERATION"},
    {GL_STACK_OVERFLOW, "STACK OVERFLOW"},
    {GL_STACK_UNDERFLOW, "STACK UNDERLOW"},
    {GL_OUT_OF_MEMORY, "OUT OF MEMORY"},
    {GL_INVALID_FRAMEBUFFER_OPERATION, "INVALID FRAMEBUFFER OPERATION"}};

static std::map<cl_int, std::string> CLErrors{
    {CL_SUCCESS, "SUCCESS"},

    {CL_DEVICE_NOT_FOUND, "DEVICE NOT FOUND"},
    {CL_DEVICE_NOT_AVAILABLE, "DEVICE NOT AVAILABLE"},
    {CL_COMPILER_NOT_AVAILABLE, "COMPILER NOT AVAILABLE"},
    {CL_MEM_OBJECT_ALLOCATION_FAILURE, "MEM OBJECT ALLOCATION FAILURE"},
    {CL_OUT_OF_RESOURCES, "OUT OF RESOURCES"},
    {CL_OUT_OF_HOST_MEMORY, "OUT OF HOST MEMORY"},
    {CL_PROFILING_INFO_NOT_AVAILABLE, "PROFILING INFO NOT AVAILABLE"},
    {CL_MEM_COPY_OVERLAP, "MEM COPY OVERLAP"},
    {CL_IMAGE_FORMAT_MISMATCH, "IMAGE FORMAT MISMATCH"},
    {CL_IMAGE_FORMAT_NOT_SUPPORTED, "IMAGE FORMAT NOT SUPPORTED"},
    {CL_BUILD_PROGRAM_FAILURE, "BUILD PROGRAM FAILURE"},
    {CL_MAP_FAILURE, "MAP FAILURE"},

    {CL_INVALID_VALUE, "INVALID VALUE"},
    {CL_INVALID_DEVICE_TYPE, "INVALID DEVICE TYPE"},
    {CL_INVALID_PLATFORM, "INVALID PLATFORM"},
    {CL_INVALID_DEVICE, "INVALID DEVICE"},
    {CL_INVALID_CONTEXT, "INVALID CONTEXT"},
    {CL_INVALID_QUEUE_PROPERTIES, "INVALID QUEUE PROPERTIES"},
    {CL_INVALID_COMMAND_QUEUE, "INVALID COMMAND QUEUE"},
    {CL_INVALID_HOST_PTR, "INVALID HOST PTR"},
    {CL_INVALID_MEM_OBJECT, "INVALID MEM OBJECT"},
    {CL_INVALID_IMAGE_FORMAT_DESCRIPTOR, "INVALID IMAGE FORMAT DESCRIPTOR"},
    {CL_INVALID_IMAGE_SIZE, "INVALID IMAGE SIZE"},
    {CL_INVALID_SAMPLER, "INVALID SAMPLER"},
    {CL_INVALID_BINARY, "INVALID BINARY"},
    {CL_INVALID_BUILD_OPTIONS, "INVALID BUILD OPTIONS"},
    {CL_INVALID_PROGRAM, "INVALID PROGRAM"},
    {CL_INVALID_PROGRAM_EXECUTABLE, "INVALID PROGRAM EXECUTABLE"},
    {CL_INVALID_KERNEL_NAME, "INVALID KERNEL NAME"},
    {CL_INVALID_KERNEL_DEFINITION, "INVALID KERNEL DEFINITION"},
    {CL_INVALID_KERNEL, "INVALID KERNEL"},
    {CL_INVALID_ARG_INDEX, "INVALID ARG INDEX"},
    {CL_INVALID_ARG_VALUE, "INVALID ARG VALUE"},
    {CL_INVALID_ARG_SIZE, "INVALID ARG SIZE"},
    {CL_INVALID_KERNEL_ARGS, "INVALID KERNEL ARGS"},
    {CL_INVALID_WORK_DIMENSION, "INVALID WORK DIMENSION"},
    {CL_INVALID_WORK_GROUP_SIZE, "INVALID WORK GROUP SIZE"},
    {CL_INVALID_WORK_ITEM_SIZE, "INVALID WORK ITEM SIZE"},
    {CL_INVALID_GLOBAL_OFFSET, "INVALID GLOBAL OFFSET"},
    {CL_INVALID_EVENT_WAIT_LIST, "INVALID EVENT WAIT LIST"},
    {CL_INVALID_EVENT, "INVALID EVENT"},
    {CL_INVALID_OPERATION, "INVALID OPERATION"},
    {CL_INVALID_GL_OBJECT, "INVALID GL OBJECT"},
    {CL_INVALID_BUFFER_SIZE, "INVALID BUFFER SIZE"},
    {CL_INVALID_MIP_LEVEL, "INVALID MIP LEVEL"},
    {CL_INVALID_GLOBAL_WORK_SIZE, "INVALID GLOBAL WORK SIZE"}};

static std::unordered_set<std::string> loggedWarnings;
static std::mutex loggerMutex;

void Logger::log(LogLevel level, const std::string &category, const std::string &msg, const int errorCode)
{
  std::lock_guard<std::mutex> lock(loggerMutex);
  std::string key = category + ":" + msg;

  if (level == LogLevel::Warning)
  {
    if (loggedWarnings.find(key) != loggedWarnings.end())
      return; // Already logged
    loggedWarnings.insert(key);
  }

  switch (level)
  {
  case LogLevel::Info:
    std::cout << "[" << category << "] INFO: " << msg << std::endl;
    break;
  case LogLevel::Warning:
    std::cerr << "[" << category << "] WARNING: " << msg << std::endl;
    break;
  case LogLevel::Error:
    std::cerr << "[" << category << "] ERROR: " << msg << std::endl;
    break;
  case LogLevel::Fatal:
    throw std::runtime_error("[" + category + "] FATAL ERROR: " + msg);
    break;
  case LogLevel::GLError:
    std::cerr << "[" << category << "] " << GLErrors[errorCode] << " ERROR: " << msg << std::endl;
    break;
  case LogLevel::CLError:
    std::cerr << "[" << category << "] " << CLErrors[errorCode] << " ERROR: " << msg << std::endl;
    break;
  }
}

void Logger::logInfo(const std::string &category, const std::string &msg)
{
  log(LogLevel::Info, category, msg);
}

void Logger::logWarning(const std::string &category, const std::string &msg)
{
  log(LogLevel::Warning, category, msg);
}

void Logger::logError(const std::string &category, const std::string &msg)
{
  log(LogLevel::Error, category, msg);
}

void Logger::logCLError(const std::string &category, const std::string &msg, const int errorCode)
{
  log(LogLevel::CLError, category, msg, errorCode);
}

void Logger::logFatal(const std::string &category, const std::string &msg)
{
  log(LogLevel::Fatal, category, msg);
}

bool Logger::checkGLError(const std::string &category, const std::string &msg)
{
  GLenum errorCode;

  bool error = false;

  while ((errorCode = glGetError()) != GL_NO_ERROR)
  {
    Logger::log(LogLevel::GLError, category, msg, errorCode);
    error = true;
  }

  return error;
}