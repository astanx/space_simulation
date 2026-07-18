#include "real.cl"
#include "quaternion.cl"
#include "lod/instanceStructs.cl"
#include "lod/lodHelper.cl"

__kernel partitionObjects(
  __global InstanceModelMatrixParts* fullInstances,
  __global InstancePositionRadiusTexture* impostorInstances,
  __global InstancePositionRadiusColor* pointInstances,
  __global real3* positions, __global dquat* orientations, __global float* instanceRadii, 
  __global real* meanRadii, __global real* polarRadii, __global real* equatorianRadii, 
  __global float3* instanceColor, __global uint* instanceTextureLayer, __global float* instanceImportance,
  float fov, float viewportHeight, float baseMinPixelSize, FrustumGPU frustum)
{
  int id = get_global_id(0);

  float importance = instanceImportance[id];
  real meanRadius = meanRadii[id];
  real3 pos = positions[id];

  float scaledMeanRadius = scaleRadius(pos, meanRadius, fov, viewportHeight, importance, baseMinPixelSize);
  instanceRadii[id] = scaledMeanRadius;

  if (!shouldBeProcessed(frustum, pos, scaledMeanRadius)) return;

  int level = getLODLevel(pos, radius, fov, viewportHeight, importance);

  switch (level)
  {
  case LOD_FULL:
  {
    real equatorianRadius = equatorianRadii[id];
    real polarRadius = polarRadii[id];
    float scaledEquatorianRadius = scaleRadius(pos, equatorianRadius, fov, viewportHeight, importance);
    float scaledPolarRadius = scaleRadius(pos, polarRadius, fov, viewportHeight, importance);

    float equatorian = scaledEquatorianRadius / equatorianRadius;
    float polar = scaledPolarRadius / polarRadius;

    quat orientation = orientations[id]; // modify later

    InstanceModelMatrixParts instance;
    instance.position = pos;
    instance.orientation = orientation;
    instance.scale = (float3)(equatorian, polar, equatorian);
    fullInstances[id] = instance;
    break;
  }
  case LOD_IMPOSTOR:
  {
    InstancePositionRadiusTexture instance;
    instance.position = pos;
    instance.radius = scaledMeanRadius;
    instance.textureLayer = instanceTextureLayer[id];
    impostorInstances[id] = instance;
    break;
  }
  case LOD_POINT:
  {
    InstancePositionRadiusColor instance;
    instance.position = pos;
    instance.radius = scaledMeanRadius;
    instance.color = instanceColor[id];
    pointInstances[id] = instance;
    break;
  }
  default:
    break;
  }
}