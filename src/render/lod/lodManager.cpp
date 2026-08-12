#include "render/lod/manager/lodManager.h"

#include "render/lod/lodFunctions.h"
#include "render/lod/lodResult.h"

#include "render/frustum.h"

#include "physics/structs/radii.h"

// Private functions
uint LODManager::getLODLevel(float pixelRadius)
{
  return ::getLODLevelFromPixelRadius(pixelRadius, this->settings.fullThreshold, this->settings.impostorThreshold);
}

uint LODManager::getLODLevel(const glm::vec3 &position, float radius, float fov, float viewportHeight, float importance)
{
  return ::getLODLevel(position, radius, fov, viewportHeight, importance, this->settings.baseMinPixelSize, this->settings.fullThreshold, this->settings.impostorThreshold);
}

// Public functions
float LODManager::calculatePixelRadius(const glm::vec3 &position, float radius, float fov, float viewportHeight, float importance)
{
  return ::calculatePixelRadius(position, radius, fov, viewportHeight, importance, this->settings.baseMinPixelSize);
}

float LODManager::scaleRadius(const glm::vec3 &position, float radius, float fov, float viewportHeight, float importance)
{
  return ::scaleRadius(position, radius, fov, viewportHeight, importance, this->settings.baseMinPixelSize);
}

LODResult LODManager::partitionObject(const glm::vec3 &position, float importance, Radii radii, Frustum *frustum, float viewportHeight, float fov)
{
  LODResult result;
  float radius = radii.mean;
  result.scaledMeanRadius = this->scaleRadius(position, radius, fov, viewportHeight, importance);

  if (!shouldBeProcessed(frustum, position, result.scaledMeanRadius))
  {
    result.visible = false;
    return result;
  }

  result.visible = true;

  result.level = this->getLODLevel(position, radius, fov, viewportHeight, importance);

  if (result.level == LOD_FULL)
  {
    result.scaledEquatorianRadius = this->scaleRadius(position, radii.equatorian, fov, viewportHeight, importance);
    result.scaledPolarRadius = this->scaleRadius(position, radii.polar, fov, viewportHeight, importance);

    result.equatorianScale = result.scaledEquatorianRadius / radii.equatorian;
    result.polarScale = result.scaledPolarRadius / radii.polar;
  }

  return result;
}