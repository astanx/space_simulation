#pragma once

#include "render/lod/lodSettings.h"

#include "graphics/texture.h"
#include "graphics/mesh.h"

#include <vector>
#include <glm/glm.hpp>

class Camera;
class Scene;
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

  bool isImpostorInitialized = false;
  std::unique_ptr<Texture> impostorTexture;
  std::unique_ptr<Mesh> impostorMesh;
  std::unique_ptr<Mesh> pointMesh;

  void initLOD(std::vector<ModelSource *> &modelSources, std::vector<RenderSystem *> &renderSystems);
  void initImpostor(std::vector<ModelSource *> &modelSources, std::vector<RenderSystem *> &renderSystems);
  void initPoint();
  void bindLayerToImpostorTexture(Model &model, unsigned int layer);
  void bindLayerToImpostorTexture(Model *model, unsigned int layer);

  int getLODLevel(float pixelRadius);
  int getLODLevel(const glm::vec3 &position, float radius, float fov, float viewportHeight, float importance = 1.f);

public:
  LODManager(LODSettings &settings) : settings(settings) {}
  ~LODManager() = default;

  void init(std::vector<ModelSource *> &modelSources, std::vector<RenderSystem *> &renderSystems);

  LODResult partitionObject(const glm::vec3 &position, float importance, Radii radii, Frustum *frustum, float viewportHeight, float fov);

  float calculatePixelRadius(const glm::vec3 &position, float radius, float fov, float viewportHeight, float importance = 1.f);
  float scaleRadius(const glm::vec3 &position, float radius, float fov, float viewportHeight, float importance = 1.f);

  Mesh &getImpostorMesh() { return *this->impostorMesh.get(); };
  Texture &getImpostorTexture() { return *this->impostorTexture.get(); };
  Mesh &getPointMesh() { return *this->pointMesh.get(); };
};