#pragma once

#include "resources/precision.h"
#include "resources/data/date.h"

#include "core/window/windowConfig.h"

#include "debug/validators/validatorConfig.h"

#include <ctime>
#include <filesystem>

enum class Backend
{
  CPU,
  GPU
};

enum class Mode
{
  EnergyValidation,
  Simulation
};

struct AppConfig
{
  WindowConfig windowConfig;
  ValidatorConfig validatorCfg;

  Mode mode = Mode::Simulation;

  Backend backend = Backend::CPU;          // --gpu or --cpu
  Precision precision = Precision::DOUBLE; // --float or --double

  double timestep = 86400;           // --timestep time
  Date startDate = Date{1, 1, 2000}; // --date day/month/year hour:minute:second
};