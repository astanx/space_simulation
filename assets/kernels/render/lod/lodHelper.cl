#ifndef LOD_HELPER
#define LOD_HELPER

#include "render/lod/lodFunctions.h"

#include "real.cl"
#include "graphics/instanceStructs.h"
#include "maths/constants.h"

uint findModelID(uint globalID, __global uint* modelRangeStart, __global uint* modelRangeEnd, uint rangeCount)
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