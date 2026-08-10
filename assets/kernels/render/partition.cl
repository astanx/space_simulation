#include "real.cl"
#include "quaternion.cl"
#include "render/lod/lodHelper.cl"
#include "graphics/instanceStructs.h"
#include "camera/worldToView.h"

__kernel void partitionObjects(
  __global InstanceModelMatrixParts* fullInstances,
  __global InstancePositionRadiusTexture* impostorInstances,
  __global InstancePositionRadiusColor* pointInstances,
  __global uint* isFull, __global uint* isNonFull, __global uint* isImpostor, __global uint* isPoint,
  __global uint* fullOffset, __global uint* nonFullOffset, __global uint* impostorOffset, __global uint* pointOffset,
  __global real3* positions, __global dquat* orientations,
  __global real* meanRadii, __global real* polarRadii, __global real* equatorianRadii,
  __global float3* modelColor, __global uint* modelTextureLayer, __global float* modelImportance,
  __global uint* modelRangeStart, __global uint* modelRangeEnd, __global uint* modelFullCount, __global uint* impostorCount, __global uint* pointCount, 
  uint rangeCount, float fov, float viewportHeight, float baseMinPixelSize, real3 camPosition
  )
{
  uint id = get_global_id(0);

  real3 pos = worldToViewSpaceVec(positions[id], camPosition);
  uint modelID = findModelID(id, modelRangeStart, modelRangeEnd, rangeCount);

  float importance = modelImportance[modelID];
  float scaledMeanRadius = scaleRadius(pos, meanRadii[id], fov, viewportHeight, importance, baseMinPixelSize);

  if (id == 0)
  {
    pointCount[0] = pointOffset[get_global_size(0) - 1] + isPoint[get_global_size(0) - 1];
    impostorCount[0] = impostorOffset[get_global_size(0) - 1] + isImpostor[get_global_size(0) - 1];
  }

  if (id == modelRangeStart[modelID])
   modelFullCount[modelID] = fullOffset[modelRangeEnd[modelID] - 1] + isFull[modelRangeEnd[modelID] - 1] - fullOffset[modelRangeStart[modelID]];

  if (isFull[id] || isNonFull[id])
  {
    real equatorianRadius = equatorianRadii[id];
    real polarRadius = polarRadii[id];

    float scaledEquatorianRadius = scaleRadius(pos, equatorianRadius, fov, viewportHeight, importance, baseMinPixelSize);
    float scaledPolarRadius = scaleRadius(pos, polarRadius, fov, viewportHeight, importance, baseMinPixelSize);

    float equatorian = scaledEquatorianRadius / equatorianRadius;
    float polar = scaledPolarRadius / polarRadius;

    quat orientation = worldToViewSpaceQuat(orientations[id]);

    InstanceModelMatrixParts instance;
    instance.position = (float3)(pos);
    instance.orientation = orientation;
    instance.scale = (float3)(equatorian, polar, equatorian);
    if (isFull[id])
    {
      uint localOffset = fullOffset[id] - fullOffset[modelRangeStart[modelID]];
      fullInstances[modelRangeStart[modelID] + localOffset] = instance;
    }
    else if (isNonFull[id])
    {
      uint localOffset = nonFullOffset[id] - nonFullOffset[modelRangeStart[modelID]];
      fullInstances[modelRangeStart[modelID] + modelFullCount[modelID]] = instance;
    }
  }

  if (isImpostor[id])
  {
    InstancePositionRadiusTexture instance;
    instance.position = (float3)(pos);
    instance.radius = scaledMeanRadius;
    instance.textureLayer = modelTextureLayer[modelID];
    impostorInstances[impostorOffset[id]] = instance;
  }

  if (isPoint[id])
  {
    InstancePositionRadiusColor instance;
    instance.position = (float3)(pos);
    instance.radius = scaledMeanRadius;
    instance.color = modelColor[modelID];
    pointInstances[pointOffset[id]] = instance;
  }
}