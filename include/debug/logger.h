#pragma once
#include <string>

enum class LogLevel
{
  Info,
  Warning,
  Error,
  Fatal
};

class Logger
{
public:
  static void log(LogLevel level, const std::string &category, const std::string &msg);
  static void logInfo(const std::string &category, const std::string &msg);
  static void logWarning(const std::string &category, const std::string &msg);
  static void logError(const std::string &category, const std::string &msg);
  static void logFatal(const std::string &category, const std::string &msg);
};
