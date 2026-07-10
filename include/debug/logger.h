#pragma once

#include <string>

#ifndef NDEBUG
#define GL_CALL(x)                                                         \
  do                                                                       \
  {                                                                        \
    while (glGetError() != GL_NO_ERROR)                                    \
    {                                                                      \
    }                                                                      \
    x;                                                                     \
    Logger::checkGLError(__FILE__, "at line " + std::to_string(__LINE__)); \
  } while (0)

#define CL_CALL(x)                                                                 \
  do                                                                               \
  {                                                                                \
    cl_int errNum = (x);                                                           \
    if (errNum != CL_SUCCESS)                                                      \
      Logger::logCLError(__FILE__, "at line " + std::to_string(__LINE__), errNum); \
  } while (0)

#define CL_CREATE(x, errNum)                                                       \
  do                                                                               \
  {                                                                                \
    x;                                                                             \
    if (errNum != CL_SUCCESS)                                                      \
      Logger::logCLError(__FILE__, "at line " + std::to_string(__LINE__), errNum); \
  } while (0)
#else
#define GL_CALL(x) x;
#define CL_CALL(x) x;
#define CL_CREATE(x) x;
#endif

enum class LogLevel
{
  Info,
  Warning,
  Error,
  Fatal,
  GLError,
  CLError
};

class Logger
{
public:
  static void log(LogLevel level, const std::string &category, const std::string &msg, const int errorCode = 0);
  static void logInfo(const std::string &category, const std::string &msg);
  static void logWarning(const std::string &category, const std::string &msg);
  static void logError(const std::string &category, const std::string &msg);
  static void logCLError(const std::string &category, const std::string &msg, const int errorCode);
  static void logFatal(const std::string &category, const std::string &msg);
  static bool checkGLError(const std::string &category, const std::string &msg);
};
