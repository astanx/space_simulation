#include "debug/logger.h"

#include "GL/glew.h"

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

static std::unordered_set<std::string> loggedWarnings;
static std::mutex loggerMutex;

void Logger::log(LogLevel level, const std::string &category, const std::string &msg, const unsigned int glError)
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
    std::cerr << "[" << category << "] FATAL ERROR: " << msg << std::endl;
    break;
  case LogLevel::GLError:
    std::cerr << "[" << category << "] " << GLErrors[glError] << " ERROR: " << msg << std::endl;
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