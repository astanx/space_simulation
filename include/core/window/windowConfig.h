#pragma once

#include <string>

struct WindowConfig
{
  std::string title = "Space Simulation";
  int width = 800;
  int height = 600;
  int GLmajor = 4;
  int GLminor = 1;
  bool resizable = true;
};