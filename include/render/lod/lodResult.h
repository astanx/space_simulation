#pragma once

struct LODResult
{
  bool visible;
  uint level;

  float scaledMeanRadius;
  float scaledEquatorianRadius;
  float scaledPolarRadius;

  float equatorianScale;
  float polarScale;
};