#pragma once

#include "compute/clBuffer.h"

struct RenderGPUBuffers
{
  CLBuffer modelColorsBuffer;
  CLBuffer modelTextureLayersBuffer;
  CLBuffer modelImportancesBuffer;
  CLBuffer modelRangeStartBuffer;
  CLBuffer modelRangeEndBuffer;
  CLBuffer modelFullCountBuffer;
  CLBuffer isNonFullableBuffer;
};