#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace LOD
{
  constexpr unsigned int Full = 0;
  constexpr unsigned int Impostor = 1;
  constexpr unsigned int Point = 2;
};

struct LODSettings
{
  float baseMinPixelSize = 1.f;
  std::vector<float> pixelRadiusThreshold = {8, 3}; // >= [0] - full, >= [1] - impostor, else - point
};

class Camera;

class LODManager
{
private:
  LODSettings settings;

public:
  LODManager() = default;
  ~LODManager() = default;

  int getLODLevel(float pixelRadius);
  int getLODLevel(const glm::vec3 &position, float radius, float fov, float viewportHeight, float importance = 1.f);

  float calculatePixelRadius(const glm::vec3 &position, float radius, float fov, float viewportHeight, float importance = 1.f);
  float scaleRadius(const glm::vec3 &position, float radius, float fov, float viewportHeight, float importance = 1.f);
};