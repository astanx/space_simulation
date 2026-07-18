#pragma once

#include "graphics/texture.h"
#include "graphics/mesh.h"

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
class Scene;
class ModelSource;
class Renderable;
struct InstancePositionRadiusTexture;
struct InstancePositionRadiusColor;
struct Frustum;
struct RenderContext;

class LODManager
{
private:
  LODSettings settings;

  bool isImpostorInitialized = false;
  std::unique_ptr<Texture> impostorTexture;
  std::unique_ptr<Mesh> impostorMesh;
  std::unique_ptr<Mesh> pointMesh;

  std::unordered_map<Renderable *, int> fullInstances;
  std::unordered_map<Renderable *, int> fullTangentInstances;
  std::vector<InstancePositionRadiusTexture> impostorInstances;
  std::vector<InstancePositionRadiusColor> pointInstances;

  void initLOD(Scene &scene);
  void initImpostor(Scene &scene);
  void initPoint();
  void bindLayerToImpostorTexture(const Texture &texture, unsigned int layer);

  void partitionObjects(Scene &scene, RenderContext &ctx);
  void partitionObject(ModelSource *object, Frustum *frustum, float viewportHeight, float fov, bool force = false);

public:
  LODManager() = default;
  ~LODManager() = default;

  void init(Scene& scene);

  int getLODLevel(float pixelRadius);
  int getLODLevel(const glm::vec3 &position, float radius, float fov, float viewportHeight, float importance = 1.f);

  float calculatePixelRadius(const glm::vec3 &position, float radius, float fov, float viewportHeight, float importance = 1.f);
  float scaleRadius(const glm::vec3 &position, float radius, float fov, float viewportHeight, float importance = 1.f);

  void update(Scene &scene, RenderContext &ctx);

  std::unordered_map<Renderable *, int> &getFullInstances() { return this->fullInstances; };
  std::unordered_map<Renderable *, int> &getFullTangentInstances() { return this->fullTangentInstances; };
  Mesh &getImpostorMesh() { return *this->impostorMesh.get(); };
  Texture &getImpostorTexture() { return *this->impostorTexture.get(); };
  Mesh &getPointMesh() { return *this->pointMesh.get(); };
};