#pragma once

#include "resources/precision.h"
#include "resources/date.h"

#include <ctime>

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
  // Window
  const char *title = "Space Simulation";
  int width = 800;
  int height = 600;
  int GLmajor = 4;
  int GLminor = 1;
  bool resizable = true;

  Mode mode = Mode::Simulation;

  double timestep = 86400;           // --timestep time
  Date startDate = Date{1, 1, 2000}; // --date day/month/year hour:minute:second

  Backend backend = Backend::CPU;          // --gpu or --cpu
  Precision precision = Precision::DOUBLE; // --float or --double
};