#ifndef LOD_HELPER
#define LOD_HELPER

#include "real.cl"
#include "lod/instanceStructs.cl"

constant uint FULL = 0;
constant uint IMPOSTOR = 1;
constant uint POINT = 2;

float scaleRadius(real3 position, real radius, float fov, float viewportHeight, float importance, float baseMinPixelSize)
{
  float minPixelSize = baseMinPixelSize * importance;

  float pixelWorldSize = (length(position) * 2.f * tan(fov / 2.f)) / viewportHeight;
  float minWorldRadius = minPixelSize * pixelWorldSize * 0.5f;

  float finalRadius = max(radius, minWorldRadius);

  return finalRadius;
}

bool isVisibleSphere(const FrustumGPU* frustum, real3 pos, float radius)
{
  for (int face = 0; face < 6; face++)
  {
    float4 plane = frustum->planes[face];
    float d = dot(plane.xyz, center) + plane.w;
    if (d < -radius)
      return false;
  }
  return true;
}

bool shouldBeProcessed(const FrustumGPU* frustum, real3 pos, float radius)
{
  if (!frustum || !isVisibleSphere(frustum, center, radius))
    return false;

  return true;
}

#endif