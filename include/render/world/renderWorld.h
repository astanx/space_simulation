#pragma once

#include "render/world/backend/renderWorldBackend.h"
#include "render/world/data/renderDatabase.h"
#include "render/world/data/renderGPUBuffers.h"

#include "render/lod/lodRenderResourcesManager.h"
#include "render/instanceManager.h"

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
class EntityManager;
struct FrameContext;
struct RenderContext;
struct RenderDatabase;
struct SharedGPUBuffers;
struct Total;
struct SharedDatabaseView;

class RenderWorld
{
private:
  LODRenderResourcesManager lodResourceManager;
  InstanceManager instanceManager;

  RenderDatabase database;
  RenderGPUBuffers gpuBuffers;
  uint32_t rangeCount;

  std::unique_ptr<RenderWorldBackend> backend;

  std::vector<Model *> models;

  Camera *activeCamera;
  Skybox *skybox;

  std::vector<std::unique_ptr<Camera>> cameras;
  std::vector<Camera *> cameraViews;

  std::vector<std::unique_ptr<Skybox>> skyboxes;
  std::vector<Skybox *> skyboxesViews;

  std::unique_ptr<PointLight> pointLight;
  std::unique_ptr<DirectionalLight> directionalLight;

  // fix trails
  std::vector<std::unique_ptr<Trail>> trails;
  std::vector<Trail *> trailViews;

  void buildQueue(const Camera &camera, RenderQueue &queue, FrameContext &ctx);
  void reserveModelInstances();

public:
  RenderWorld();
  ~RenderWorld();

  void init(Total &total);
  void initCPUBackend();
  void initGPUBuffers(Context &ctx, RenderDatabase &gpu);
  void initGPUBackend(Context &ctx, CommandQueue &queue, Total &total, ResourceManager &resourceManager, SharedGPUBuffers &data);

  void update(RenderQueue &queue, FrameContext &ctx, const SharedDatabaseView &shared, const EntityManager &entityManager);
  void sync(IPhysicsWorld &physics, const SharedDatabaseView &shared, const EntityManager &entityManager);

  void renderImpostorMeshInstanced();
  void renderPointMeshInstanced();

  // Setters
  void addPointLight(std::unique_ptr<PointLight> pointLight);
  void addDirLight(std::unique_ptr<DirectionalLight> directionalLight);
  void addCamera(std::unique_ptr<Camera> camera);
  void addSkybox(std::unique_ptr<Skybox> skybox);

  void increaseCameraSpeed(double percentage = 10.0);
  void decreaseCameraSpeed(double percentage = 10.0);

  void addTrail(std::unique_ptr<Trail> trail);

  void setDatabase(RenderDatabase database) { this->database = database; };

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