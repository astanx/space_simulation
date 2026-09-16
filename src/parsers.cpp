#include "parsers.h"

#include "debug/logger.h"

#include "core/app/appConfig.h"

#include "resources/data/date.h"

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
    cfg.timeCfg.timestep = scale * 86400;
    return true;
  }
  else if (factor == "m")
  {
    cfg.timeCfg.timestep = scale * 86400 * 30.6001;
    return true;
  }
  else if (factor == "y")
  {
    cfg.timeCfg.timestep = scale * 86400 * 365.2425;
    return true;
  }
  else if (factor == "h")
  {
    cfg.timeCfg.timestep = scale * 60 * 24;
    return true;
  }
  else if (factor == "min")
  {
    cfg.timeCfg.timestep = scale * 60;
    return true;
  }
  else
  {
    cfg.timeCfg.timestep = scale;
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

bool parseSaveFolder(AppConfig &cfg, std::string param)
{
  if (param.starts_with("--"))
  {
    Logger::logError("Parsers", "Wrong use of --save argument");
    return false;
  }

  std::filesystem::path path = param;

  if (path.empty())
  {
    Logger::logError("Parsers", "Empty folder path specified for --save");
    return false;
  }

  if (!std::filesystem::exists(path))
  {
    Logger::logInfo("Parsers", "Folder does not exist, creating: " + path.string());

    try
    {
      std::filesystem::create_directories(path);
    }
    catch (const std::exception &e)
    {
      Logger::logError("Parsers", "Cannot create folder: " + path.string());
      return false;
    }
  }

  if (!std::filesystem::is_directory(path))
  {
    Logger::logError("Parsers", "Path is not a directory: " + path.string());
    return false;
  }

  cfg.validatorCfg.pathSpecified = true;
  cfg.validatorCfg.savePath = path;

  return true;
}

bool parseSteps(AppConfig &cfg, std::string param)
{
  if (param.starts_with("--"))
  {
    Logger::logError("Parsers", "Wrong use of --steps argument");
    return false;
  }

  size_t steps;
  try
  {
    steps = std::stoi(param);
  }
  catch (const std::exception &e)
  {
    Logger::logError("Parsers", "Couldnt parse steps for --steps argument");
    return false;
  }

  cfg.validatorCfg.steps = steps;

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

    cfg.timeCfg.startDate = Date{d, m, y, h, min, s};
  }

  return true;
}

bool parsePrecision(Precision &precision, std::string param)
{
  if (param == "double")
  {
    precision = Precision::DOUBLE;
    return true;
  }
  else if (param == "float")
  {
    precision = Precision::FLOAT;
    return true;
  }
  else
  {
    Logger::logError("Parsers", "Unknown precision argument passed, defaulting to double");
    precision = Precision::DOUBLE;
    return false;
  }
}

bool parseWorldPrecision(AppConfig &cfg, std::string param)
{
  if (param.starts_with("-"))
  {
    Logger::logError("Parsers", "Wrong use of --precision argument");
    return false;
  }

  return parsePrecision(cfg.worldCfg.precision, param);
}
bool parseValidatorPrecision(AppConfig &cfg, std::string param)
{
  if (param.starts_with("-"))
  {
    Logger::logError("Parsers", "Wrong use of --validator-precision argument");
    return false;
  }

  return parsePrecision(cfg.validatorCfg.precision, param);
}
bool parseForceModel(AppConfig &cfg, std::string param)
{
  if (param.starts_with("-"))
  {
    Logger::logError("Parsers", "Wrong use of --force-model argument");
    return false;
  }

  if (param == "direct")
    cfg.worldCfg.physics.forceModel = PhysicsForceModel::Direct;
  else if (param == "bh")
    cfg.worldCfg.physics.forceModel = PhysicsForceModel::BarnesHut;
  else
    Logger::logError("Parsers", "Wrong --force-model argument passed");

  return true;
}
bool parseIntegrator(AppConfig &cfg, std::string param)
{
  if (param.starts_with("-"))
  {
    Logger::logError("Parsers", "Wrong use of --integrator argument");
    return false;
  }

  if (param == "wh")
    cfg.worldCfg.physics.integrator = PhysicsIntegrator::WisdomHolman;
  else
    Logger::logError("Parsers", "Wrong --integrator argument passed");

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
    else if (arg == "--validate-energy")
      cfg.mode = Mode::EnergyValidation;
    else if (arg == "--simulation")
      cfg.mode = Mode::Simulation;
    else if (arg == "--force-model")
    {
      if (argc <= i + 1)
      {
        Logger::logError("Parsers", "Wrong use of --force-model argument");
        continue;
      }

      if (parseForceModel(cfg, argv[i + 1]))
        i++;
    }
    else if (arg == "--integrator")
    {
      if (argc <= i + 1)
      {
        Logger::logError("Parsers", "Wrong use of --integrator argument");
        continue;
      }

      if (parseIntegrator(cfg, argv[i + 1]))
        i++;
    }
    else if (arg == "--precision")
    {
      if (argc <= i + 1)
      {
        Logger::logError("Parsers", "Wrong use of --precision argument");
        continue;
      }

      if (parseWorldPrecision(cfg, argv[i + 1]))
        i++;
    }
    else if (arg == "--validator-precision")
    {
      if (argc <= i + 1)
      {
        Logger::logError("Parsers", "Wrong use of --validator-precision argument");
        continue;
      }

      if (parseValidatorPrecision(cfg, argv[i + 1]))
        i++;
    }
    else if (arg == "--save")
    {
      if (argc <= i + 1)
      {
        Logger::logError("Parsers", "Wrong use of --save argument");
        continue;
      }

      if (parseSaveFolder(cfg, argv[i + 1]))
        i++;
    }
    else if (arg == "--steps")
    {
      if (argc <= i + 1)
      {
        Logger::logError("Parsers", "Wrong use of --steps argument");
        continue;
      }

      if (parseSteps(cfg, argv[i + 1]))
        i++;
    }
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