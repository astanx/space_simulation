#pragma once

#include <string>

#ifndef NDEBUG
#define GL_CALL(x)                                                        \
  do                                                                      \
  {                                                                       \
    while (glGetError() != GL_NO_ERROR)                                   \
    {                                                                     \
    }                                                                     \
    x;                                                                    \
    Logger::checkGLError(__FILE__, "at line " + std::to_string(__LINE__)); \
  } while (0)
#else
#define GL_CALL(x) x;
#endif

enum class LogLevel
{
  Info,
  Warning,
  Error,
  Fatal,
  GLError
};

class Logger
{
public:
  static void log(LogLevel level, const std::string &category, const std::string &msg, const unsigned int glError = 0);
  static void logInfo(const std::string &category, const std::string &msg);
  static void logWarning(const std::string &category, const std::string &msg);
  static void logError(const std::string &category, const std::string &msg);
  static void logFatal(const std::string &category, const std::string &msg);
  static bool checkGLError(const std::string &category, const std::string &msg);
};
