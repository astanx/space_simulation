#include "real.cl"

#include "render/frustum.h"
#include "render/findModelID.h"
#include "camera/worldToView.h"

__kernel void lodPass(
  __global uint* isFull, __global uint* isNotFull, __global uint* isImpostor, __global uint* isPoint, __global real3* positions, 
  __global real* meanRadii, __global float* modelImportance, 
  __global uint* modelRangeStart, __global uint* modelRangeEnd,
  __global uint* isNonFullable,
  uint rangeCount,  float fov, float viewportHeight, float baseMinPixelSize, 
  float fullThreshold, float impostorThreshold, uint count, Frustum frustum, real3 camPosition)
{
  uint id = get_global_id(0);

  if (id >= count) return;

  isFull[id] = 0;
  isNotFull[id] = 0;
  isImpostor[id] = 0;
  isPoint[id] = 0;

  uint modelID = findModelID(id, modelRangeStart, modelRangeEnd, rangeCount);

  float importance = modelImportance[modelID];
  real meanRadius = meanRadii[id];
  real3 pos = worldToViewSpaceVec(positions[id], camPosition);

  float scaledMeanRadius = scaleRadius(pos, meanRadius, fov, viewportHeight, importance, baseMinPixelSize);

  if (isNonFullable[modelID])
    isNotFull[id] = 1;

  if (!shouldBeProcessed(&frustum, pos, scaledMeanRadius)) return;

  uint level = getLODLevel(pos, meanRadius, fov, viewportHeight, importance, baseMinPixelSize, fullThreshold, impostorThreshold);
  
  switch (level)
  {
  case LOD_FULL:
  {
    isFull[id] = 1;
    isNotFull[id] = 0;
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