#pragma once

struct LODResult
{
  bool visible;
  int level;

  float scaledMeanRadius;
  float scaledEquatorianRadius;
  float scaledPolarRadius;

  float equatorianScale;
  float polarScale;
};

namespace LOD
{
  constexpr unsigned int Full = 0;
  constexpr unsigned int Impostor = 1;
  constexpr unsigned int Point = 2;
};