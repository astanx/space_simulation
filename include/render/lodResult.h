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