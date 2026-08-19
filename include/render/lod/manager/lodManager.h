#pragma once

#include "render/lod/lodSettings.h"

#include "graphics/texture.h"
#include "graphics/mesh.h"

#include <vector>
#include <glm/glm.hpp>

class Camera;
class Model;
class ModelSource;
class Renderable;
class RenderSystem;
struct InstancePositionRadiusTexture;
struct InstancePositionRadiusColor;
struct Frustum;
struct RenderContext;
struct LODResult;
struct Radii;

class LODManager
{
private:
  LODSettings &settings;

  uint getLODLevel(float pixelRadius);
  uint getLODLevel(const glm::vec3 &position, float radius, float fov, float viewportHeight, float importance = 1.f);

public:
  LODManager(LODSettings &settings) : settings(settings) {}
  ~LODManager() = default;

  LODResult partitionObject(const glm::vec3 &position, float importance, Radii radii, Frustum *frustum, float viewportHeight, float fov);

  float calculatePixelRadius(const glm::vec3 &position, float radius, float fov, float viewportHeight, float importance = 1.f);
  float scaleRadius(const glm::vec3 &position, float radius, float fov, float viewportHeight, float importance = 1.f);
};