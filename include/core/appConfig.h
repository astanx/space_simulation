#pragma once

#include "resources/precision.h"

enum class Backend
{
  CPU,
  GPU
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

  Backend backend = Backend::CPU;         // --gpu or --cpu
  Precision precision = Precision::DOUBLE; // --float or --double
};