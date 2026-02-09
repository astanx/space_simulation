#include "debug/logger.h"
#include <iostream>
#include <unordered_set>
#include <mutex>

static std::unordered_set<std::string> loggedWarnings;
static std::mutex loggerMutex;

void Logger::log(LogLevel level, const std::string &category, const std::string &msg)
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