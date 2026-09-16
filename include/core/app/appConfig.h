#pragma once

#include "resources/data/precision.h"
#include "resources/data/date.h"

#include "core/window/windowConfig.h"
#include "core/time/timeConfig.h"
#include "scene/world/worldConfig.h"

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
  TimeConfig timeCfg;
  WorldConfig worldCfg;

  Mode mode = Mode::Simulation;

  Backend backend = Backend::CPU;          // --gpu or --cpu
  Precision precision = Precision::DOUBLE; // --precision <float|double>
};