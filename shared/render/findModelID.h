#ifndef FIND_MODEL_H
#define FIND_MODEL_H

#ifdef __OPENCL_VERSION__
#else
#include <vector>
using uint = uint32_t;
#endif

#include "render/lod/lodFunctions.h"
#include "graphics/instanceStructs.h"
#include "maths/constants.h"

#ifdef __OPENCL_VERSION__
uint findModelID(uint globalID, __global uint *modelRangeStart, __global uint *modelRangeEnd, uint rangeCount)
#else
uint findModelID(uint globalID, const std::vector<uint> &modelRangeStart, const std::vector<uint> &modelRangeEnd, uint rangeCount)
#endif
{
  uint left = 0;
  uint right = rangeCount - 1;

  while (left != right)
  {
    uint mid = (right + left) / 2;
    if (globalID >= modelRangeEnd[mid])
      left = mid + 1;
    else if (globalID < modelRangeStart[mid])
      right = mid - 1;
    else
      return mid;
  }

  return left;
}

#endif