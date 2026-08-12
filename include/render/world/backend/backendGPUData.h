#pragma once

#include <cstdint>

class CLBuffer;

struct BackendGPUData
{
  CLBuffer &positions;
  CLBuffer &orientations;
  CLBuffer &meanRadii;
  CLBuffer &polarRadii;
  CLBuffer &equatorianRadii;
  CLBuffer &modelImportances;
  CLBuffer &modelColors;
  CLBuffer &modelTextureLayers;
  CLBuffer &fullInstances;
  CLBuffer &impostorInstances;
  CLBuffer &pointInstances;
  CLBuffer &modelRangeStart;
  CLBuffer &modelRangeEnd;

  uint32_t rangeCount;
};