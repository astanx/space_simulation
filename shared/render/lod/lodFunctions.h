#ifndef LOD_HELPER_H
#define LOD_HELPER_H

#ifdef __OPENCL_VERSION__
#include "real.cl"
#else
#include <glm/glm.hpp>
#include <cstdint>
using uint = uint32_t;
using real3 = glm::vec3;
using glm::length;
using glm::radians;
using std::max;
#endif

#include "maths/constants.h"
#include "render/lod/lodConstants.h"

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

float scaleRadius(real3 position, float radius, float fov, float viewportHeight, float importance, float baseMinPixelSize)
{
  float minPixelSize = baseMinPixelSize * importance;

  float pixelWorldSize = (length(position) * 2.f * tan(radians(fov / 2.f))) / viewportHeight;
  float minWorldRadius = minPixelSize * pixelWorldSize * 0.5f;

  float finalRadius = max(radius, minWorldRadius);

  return finalRadius;
}

#endif
