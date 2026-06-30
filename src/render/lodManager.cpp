#include "render/lodManager.h"

#include "maths/constants.h"

#include "camera/camera.h"
#include <iostream>

// Public functions
int LODManager::getLODLevel(float pixelRadius)
{
  if (pixelRadius >= this->settings.pixelRadiusThreshold[LOD::Full])
    return LOD::Full;
  else if (pixelRadius >= this->settings.pixelRadiusThreshold[LOD::Impostor])
    return LOD::Impostor;
  else
    return LOD::Point;
}

int LODManager::getLODLevel(const Camera &camera, const glm::vec3 &position, float radius, float viewportHeight, float importance)
{
  float pixelRadius = this->calculatePixelRadius(camera, position, radius, viewportHeight, importance);

  return this->getLODLevel(pixelRadius);
}

float LODManager::calculatePixelRadius(const Camera &camera, const glm::vec3 &position, float radius, float viewportHeight, float importance)
{
  float d = glm::length(position);

  if (d < EPS)
    d = 1e-4f;

  float worldToPixel = (viewportHeight * 0.5f) / (d * tan(glm::radians(camera.getFOV() / 2.0f)));

  float pixelRadius = radius * worldToPixel * importance;

  float minPixel = this->settings.baseMinPixelSize * importance;
  pixelRadius = std::max(pixelRadius, minPixel);

  return pixelRadius;
}

float LODManager::scaleRadius(const Camera &camera, const glm::vec3 &position, float radius, float viewportHeight, float importance)
{
  float minPixelSize = this->settings.baseMinPixelSize * importance;

  float pixelWorldSize = (length(position) * 2.0 * tan(glm::radians(camera.getFOV() / 2.0))) / viewportHeight;
  float minWorldRadius = minPixelSize * pixelWorldSize * 0.5;

  float finalRadius = std::max(radius, minWorldRadius);

  return finalRadius;
}