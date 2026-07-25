#include "real.cl"
#include "quaternion.cl"
#include "frustum.cl"
#include "lod/instanceStructs.cl"
#include "lod/lodHelper.cl"

__kernel partitionObjects(
  __global InstanceModelMatrixParts* fullInstances,
  __global InstancePositionRadiusTexture* impostorInstances,
  __global InstancePositionRadiusColor* pointInstances,
  __global uint* isFull, __global uint* isImpostor, __global uint* isPoint,
  __global uint* fullOffset, __global uint* impostorOffset, __ global uint* pointOffset,
  __global real3* positions, __global dquat* orientations,
  __global real* meanRadii, __global real* polarRadii, __global real* equatorianRadii, 
  __global float3* instanceColor, __global uint* instanceTextureLayer, __global float* instanceImportance,
  float fov, float viewportHeight, float baseMinPixelSize, FrustumGPU frustum)
{
  int id = get_global_id(0);

  float3 pos = positions[id]; // modify later
  float scaledMeanRadius = scaleRadius(pos, meanRadii[id], fov, viewportHeight, importance, baseMinPixelSize);

  if (isFull[id])
  {
    real equatorianRadius = equatorianRadii[id];
    real polarRadius = polarRadii[id];

    float scaledEquatorianRadius = scaleRadius(pos, equatorianRadius, fov, viewportHeight, importance, baseMinPixelSize);
    float scaledPolarRadius = scaleRadius(pos, polarRadius, fov, viewportHeight, importance, baseMinPixelSize);

    float equatorian = scaledEquatorianRadius / equatorianRadius;
    float polar = scaledPolarRadius / polarRadius;

    quat orientation = orientations[id]; // modify later

    InstanceModelMatrixParts instance;
    instance.position = pos;
    instance.orientation = orientation;
    instance.scale = (float3)(equatorian, polar, equatorian);
    fullInstances[fullOffset[id]] = instance;
  }

  if (isImpostor[id])
  {
    InstancePositionRadiusTexture instance;
    instance.position = pos;
    instance.radius = scaledMeanRadius;
    instance.textureLayer = instanceTextureLayer[id];
    impostorInstances[impostorOffset[id]] = instance;
  }

  if (isPoint[id])
  {
    InstancePositionRadiusColor instance;
    instance.position = pos;
    instance.radius = scaledMeanRadius;
    instance.color = instanceColor[id];
    pointInstances[pointOffset[id]] = instance;
  }
}