#pragma once

#include "render/world/backend/renderWorldBackend.h"
#include "render/world/data/renderDatabase.h"

#include "render/lod/lodRenderResourcesManager.h"
#include "render/instanceManager.h"

#include "compute/clBuffer.h"

#include "physics/trail.h"

#include <memory>

class Updatable;
class Renderable;
class RenderSystem;
class RenderQueue;
class ModelSource;
class Camera;
class Context;
class ResourceManager;
class CommandQueue;
class Skybox;
class PointLight;
class DirectionalLight;
class IPhysicsWorld;
struct FrameContext;
struct RenderContext;
struct RenderDatabase;
struct SharedGPUBuffers;
struct Total;

class RenderWorld
{
private:
  LODRenderResourcesManager lodResourceManager;
  InstanceManager instanceManager;

  RenderDatabase database;

  std::unique_ptr<RenderWorldBackend> backend;

  Camera *activeCamera;
  Skybox *skybox;

  std::vector<std::unique_ptr<Camera>> cameras;
  std::vector<Camera *> cameraViews;

  std::vector<std::unique_ptr<Skybox>> skyboxes;
  std::vector<Skybox *> skyboxesViews;

  std::unique_ptr<PointLight> pointLight;
  std::unique_ptr<DirectionalLight> directionalLight;

  std::vector<Updatable *> updatable;
  std::vector<RenderSystem *> renderSystems;
  std::vector<ModelSource *> modelSources;

  std::vector<std::unique_ptr<Trail>> trails;
  std::vector<Trail *> trailViews;

  CLBuffer modelColorsBuffer;
  CLBuffer modelTextureLayersBuffer;
  CLBuffer modelImportancesBuffer;
  CLBuffer modelRangeStartBuffer;
  CLBuffer modelRangeEndBuffer;
  CLBuffer modelFullCountBuffer;
  CLBuffer isNonFullableBuffer;
  uint32_t rangeCount;

  std::vector<Model *> models;

  void buildQueue(const Camera &camera, RenderQueue &queue, FrameContext &ctx);
  void reserveModelInstances();

public:
  RenderWorld();
  ~RenderWorld();

  void init(ResourceManager &manager, IPhysicsWorld &physics, RenderContext &ctx, Total &total);
  void initCPUBackend();
  void initGPUBuffers(Context &ctx, RenderDatabase &gpu);
  void initGPUBackend(Context &ctx, CommandQueue &queue, Total &total, ResourceManager &resourceManager, SharedGPUBuffers &data);

  void update(RenderQueue &queue, FrameContext &ctx);
  void sync(IPhysicsWorld &physics);

  void renderImpostorMeshInstanced();
  void renderPointMeshInstanced();

  // Setters
  void addPointLight(std::unique_ptr<PointLight> pointLight);
  void addDirLight(std::unique_ptr<DirectionalLight> directionalLight);
  void addCamera(std::unique_ptr<Camera> camera);
  void addSkybox(std::unique_ptr<Skybox> skybox);

  void increaseCameraSpeed(double percentage = 10.0);
  void decreaseCameraSpeed(double percentage = 10.0);

  void addRenderSystem(RenderSystem *system);
  void addModelSource(ModelSource *object);
  void addUpdatable(Updatable *object);
  void addTrail(std::unique_ptr<Trail> trail);

  // Getters
  Camera &getActiveCamera();
  const Skybox &getActiveSkybox() const;

  const glm::vec3 getActiveCameraPosition() const;
  const PointLight *getPointLight() const;
  const DirectionalLight *getDirLight() const;

  InstanceManager &getInstanceManager() { return this->instanceManager; };
  Texture &getImpostorTexture() { return this->lodResourceManager.getImpostorTexture(); };
  Buffer &getFullInstancesVBO() { return this->instanceManager.getFullInstancesVBO(); };
  std::vector<Trail *> &getTrails();
};