#pragma once

#include "render/world/backend/backendGPUBuffers.h"

#include <cstdint>

class CLBuffer;

struct LODBuffersData
{
  CLBuffer &isFullBuffer;
  CLBuffer &isNonFullBuffer;
  CLBuffer &isImpostorBuffer;
  CLBuffer &isPointBuffer;
  CLBuffer &fullOffsetBuffer;
  CLBuffer &nonFullOffsetBuffer;
  CLBuffer &impostorOffsetBuffer;
  CLBuffer &pointOffsetBuffer;
};

struct RenderQueueGPUBuffers
{
  BackendGPUBuffers data;
  LODBuffersData lodData;
};