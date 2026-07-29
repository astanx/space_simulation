#pragma once

#include <vector>

struct LODSettings
{
  float baseMinPixelSize = 1.f;
  std::vector<float> pixelRadiusThreshold = {8, 3}; // >= [0] - full, >= [1] - impostor, else - point
};