#pragma once

#include "render/world/backend/renderWorldBackend.h"

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
struct FrameContext;
struct RenderDataGPU;
struct SharedGPUData;
struct Total;

class RenderWorld
{
private:
  LODRenderResourcesManager lodResourceManager;
  InstanceManager instanceManager;

  std::unique_ptr<RenderWorldBackend> backend;

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
  RenderWorld() = default;
  ~RenderWorld() = default;

  void init(Total &total);
  void initCPUBackend();
  void initGPUBuffers(Context &ctx, RenderDataGPU &gpu);
  void initGPUBackend(Context &ctx, CommandQueue &queue, Total& total, ResourceManager &resourceManager, SharedGPUData &data);

  void update(const Camera &camera, RenderQueue &queue, FrameContext &ctx);

  void renderImpostorMeshInstanced();
  void renderPointMeshInstanced();

  void addRenderSystem(RenderSystem *system);
  void addModelSource(ModelSource *object);
  void addUpdatable(Updatable *object);
  void addTrail(std::unique_ptr<Trail> trail);

  InstanceManager &getInstanceManager() { return this->instanceManager; };
  Texture &getImpostorTexture() { return this->lodResourceManager.getImpostorTexture(); };
  Buffer &getFullInstancesVBO() { return this->instanceManager.getFullInstancesVBO(); };
  std::vector<Trail *> &getTrails();
};