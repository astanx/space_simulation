#include "parsers.h"

#include "debug/logger.h"

#include "core/appConfig.h"

#include "resources/date.h"

#include <string>
#include <ranges>

int daysInMonth(int month, int year)
{
  switch (month)
  {
  case 1:
    return 31;
  case 2:
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0) ? 29 : 28;
  case 3:
    return 31;
  case 4:
    return 30;
  case 5:
    return 31;
  case 6:
    return 30;
  case 7:
    return 31;
  case 8:
    return 31;
  case 9:
    return 30;
  case 10:
    return 31;
  case 11:
    return 30;
  case 12:
    return 31;
  default:
    return 0;
  }
}

bool parseTimestep(AppConfig &cfg, std::string timestep)
{
  if (timestep.starts_with("--"))
  {
    Logger::logError("Parsers", "Wrong use of --timestep argument");
    return false;
  }

  size_t idx;
  double scale;
  try
  {
    scale = std::stod(timestep, &idx);
  }
  catch (const std::exception &e)
  {
    Logger::logError("Parsers", "Wrong timestep passed");
    return false;
  }

  std::string factor = timestep.substr(idx);

  if (factor == "d")
  {
    cfg.timestep = scale * 86400;
    return true;
  }
  else if (factor == "m")
  {
    cfg.timestep = scale * 86400 * 30.6001;
    return true;
  }
  else if (factor == "y")
  {
    cfg.timestep = scale * 86400 * 365.2425;
    return true;
  }
  else if (factor == "h")
  {
    cfg.timestep = scale * 60 * 24;
    return true;
  }
  else if (factor == "min")
  {
    cfg.timestep = scale * 60;
    return true;
  }
  else
  {
    cfg.timestep = scale;
    return true;
  }
}

bool parseDate(int date[3], std::string param)
{
  size_t i = 0;
  for (auto part : std::views::split(param, '/'))
  {
    if (i >= 3)
    {
      Logger::logError("Parsers", "Too many date parameters passed for --date");
      return false;
    }
    std::string s(part.begin(), part.end());
    try
    {
      date[i] = std::stoi(s);
    }
    catch (const std::exception &e)
    {
      Logger::logError("Parsers", "Wrong date parameter passed for --date");
      return false;
    }

    i++;
  }

  return true;
}

bool parseTime(int time[3], std::string param)
{
  size_t i = 0;
  for (auto part : std::views::split(param, ':'))
  {
    if (i >= 3)
    {
      Logger::logError("Parsers", "Too many time parameters passed for --date");
      return false;
    }
    std::string s(part.begin(), part.end());
    try
    {
      time[i] = std::stoi(s);
    }
    catch (const std::exception &e)
    {
      Logger::logError("Parsers", "Wrong time parameter passed for --date");
      return false;
    }

    i++;
  }

  return true;
}

bool parseStart(AppConfig &cfg, std::string firstParam, std::string secondParam)
{
  if (firstParam.starts_with("-"))
  {
    Logger::logError("Parsers", "Wrong use of --date argument");
    return false;
  }

  int time[3];
  int date[3];
  bool timeInit = false;
  bool dateInit = false;

  if (firstParam.find(":") != std::string::npos)
    timeInit = parseTime(time, firstParam);
  else if (firstParam.find("/") != std::string::npos)
    dateInit = parseDate(date, firstParam);
  else
  {
    Logger::logError("Parsers", "Wrong use of --date argument");
    return false;
  }

  if (secondParam.find(":") != std::string::npos && !timeInit)
    timeInit = parseTime(time, secondParam);
  else if (secondParam.find("/") != std::string::npos && !dateInit)
    dateInit = parseDate(date, secondParam);

  if (!dateInit)
  {
    Logger::logError("Parsers", "Wrong use of --date argument");
    return false;
  }
  else
  {
    int d, m, y;
    d = date[0];
    m = date[1];
    y = date[2];

    int h = 0;
    int min = 0;
    int s = 0;
    if (timeInit)
    {
      h = time[0];
      min = time[1];
      s = time[2];
      if (s >= 60 || s < 0)
      {
        Logger::logError("Parsers", "Wrong seconds passed to --date");
        s = 59;
      }

      if (min >= 60 || min < 0)
      {
        Logger::logError("Parsers", "Wrong minute passed to --date");
        min = 59;
      }

      if (h >= 24 || h < 0)
      {
        Logger::logError("Parsers", "Wrong hour passed to --date");
        h = 23;
      }
    }

    if (m >= 13 || m < 0)
    {
      Logger::logError("Parsers", "Wrong month passed to --date");
      m = 12;
    }

    if (d > daysInMonth(m, y) || d < 0)
    {
      Logger::logError("Parsers", "Wrong day passed to --date");
      d = daysInMonth(m, y);
    }

    cfg.startDate = Date{d, m, y, h, min, s};
  }

  return true;
}

AppConfig parseArgs(int argc, char **argv)
{
  AppConfig cfg;

  for (int i = 1; i < argc; ++i)
  {
    std::string arg = argv[i];

    if (arg == "--gpu")
      cfg.backend = Backend::GPU;
    else if (arg == "--cpu")
      cfg.backend = Backend::CPU;
    else if (arg == "--double")
      cfg.precision = Precision::DOUBLE;
    else if (arg == "--float")
      cfg.precision = Precision::FLOAT;
    else if (arg == "--timestep")
    {
      if (argc <= i + 1)
      {
        Logger::logError("Parsers", "Wrong use of --timestep argument");
        continue;
      }

      if (parseTimestep(cfg, argv[i + 1]))
        i++;
    }
    else if (arg == "--date")
    {
      if (argc <= i + 1)
      {
        Logger::logError("Parsers", "Wrong use of --date argument");
        continue;
      }
      std::string secondParam;
      size_t incrementor = 1;
      if (argc > i + 2)
      {
        secondParam = argv[i + 2];
        if (secondParam.starts_with("-"))
          secondParam = "";
        else
          incrementor++;
      }

      if (parseStart(cfg, argv[i + 1], secondParam))
        i += incrementor;
    }
    else
      Logger::logWarning("Parsers", "Unknow argument: " + arg);
  }

  return cfg;
}