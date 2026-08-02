#ifndef LOD_HELPER
#define LOD_HELPER

#include "real.cl"
#include "graphics/instanceStructs.h"
#include "maths/constants.h"

constant uint LOD_FULL = 1;
constant uint LOD_IMPOSTOR = 2;
constant uint LOD_POINT = 3;

uint getLODLevelFromPixelRadius(float pixelRadius, float fullThreshold, float impostorThreshold)
{
  if (pixelRadius >= fullThreshold)
    return LOD_FULL;
  else if (pixelRadius >= impostorThreshold)
    return LOD_IMPOSTOR;
  else
    return LOD_POINT;
}

float calculatePixelRadius(real3 position, float radius, float fov, float viewportHeight, float importance, float baseMinPixelSize)
{
  float d = length(position);

  if (d < EPS)
    d = 1e-4f;

  float worldToPixel = (viewportHeight * 0.5f) / (d * tan(radians(fov / 2.f)));

  float pixelRadius = radius * worldToPixel * importance;

  float minPixel = baseMinPixelSize * importance;
  pixelRadius = max(pixelRadius, minPixel);

  return pixelRadius;
}

uint getLODLevel(real3 position, float radius, float fov, float viewportHeight, float importance, float baseMinPixelSize, float fullThreshold, float impostorThreshold)
{
  float pixelRadius = calculatePixelRadius(position, radius, fov, viewportHeight, importance, baseMinPixelSize);

  return getLODLevelFromPixelRadius(pixelRadius, fullThreshold, impostorThreshold);
}

float scaleRadius(real3 position, real radius, float fov, float viewportHeight, float importance, float baseMinPixelSize)
{
  float minPixelSize = baseMinPixelSize * importance;

  float pixelWorldSize = (length(position) * 2.f * tan(fov / 2.f)) / viewportHeight;
  float minWorldRadius = minPixelSize * pixelWorldSize * 0.5f;

  float finalRadius = max(radius, minWorldRadius);

  return finalRadius;
}

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