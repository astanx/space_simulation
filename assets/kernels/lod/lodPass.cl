#include "real.cl"
#include "frustum.cl"
#include "lod/lodHelper.cl"

__kernel lodPass(__global uint* isFull, __global uint* isImpostor, __global uint* isPoint, __global real3* positions, __global *real meanRadii, __global float instanceImportance, float fov, float viewportHeight, float baseMinPixelSize, FrustumGPU frustum)
{
  int id = get_global_id(0);

  isFull[id] = 0;
  isImpostor[id] = 0;
  isPoint[id] = 0;

  float importance = instanceImportance[id];
  real meanRadius = meanRadii[id];
  real3 pos = positions[id];

  float scaledMeanRadius = scaleRadius(pos, meanRadius, fov, viewportHeight, importance, baseMinPixelSize);

  if (!shouldBeProcessed(frustum, pos, scaledMeanRadius)) return;

  int level = getLODLevel(pos, radius, fov, viewportHeight, importance);

  switch (level)
  {
  case LOD_FULL:
  {
    isFull[id] = 1;
    break;
  }
  case LOD_IMPOSTOR:
  {
    isImpostor[id] = 1;
    break;
  }
  case LOD_POINT:
  {
    isPoint[id] = 1;
    break;
  }
  default:
    break;
  }
}