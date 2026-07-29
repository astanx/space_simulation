#ifndef FRUSTUM
#define FRUSTUM

#include "real.cl"

typedef struct{
  float4 planes[6];
} FrustumGPU;

bool isVisibleSphere(const FrustumGPU* frustum, real3 pos, float radius)
{
  for (int face = 0; face < 6; face++)
  {
    float4 plane = frustum->planes[face];
    float d = dot(plane.xyz, pos) + plane.w;
    if (d < -radius)
      return false;
  }
  return true;
}

bool shouldBeProcessed(const FrustumGPU* frustum, real3 pos, float radius)
{
  if (!frustum || !isVisibleSphere(frustum, pos, radius))
    return false;

  return true;
}

#endif