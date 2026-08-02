#pragma once

#include "render/lod/manager/lodManager.h"
#include "render/lod/manager/lodManagerGPU.h"
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

class RenderWorld
{
private:
  LODManager lodManager;
  std::unique_ptr<LODManagerGPU> lodManagerGPU;
  LODSettings lodSettings;

  InstanceManager instanceManager;

  std::vector<Updatable *> updatable;
  std::vector<RenderSystem *> renderSystems;
  std::vector<ModelSource *> modelSources;

  std::vector<std::unique_ptr<Trail>> trails;
  std::vector<Trail *> trailViews;

  CommandQueue &queue;

  CLBuffer instanceColorsBuffer;
  CLBuffer instanceTextureLayersBuffer;
  CLBuffer instanceImportancesBuffer;
  CLBuffer modelRangeStart;
  CLBuffer modelRangeEnd;
  CLBuffer modelFullCount;
  uint32_t rangeCount;

  void buildQueue(const Camera &camera, RenderQueue &queue, FrameContext &ctx);
  void reserveModelInstances();
public:
  RenderWorld(CommandQueue &queue) : queue(queue), lodManager(lodSettings) {};
  ~RenderWorld() = default;

  void init(size_t totalObjects);
  void initGPU(Context &ctx, RenderDataGPU &gpu);
  void initLODGPU(Context &ctx, ResourceManager &resourceManager, SharedGPUData &data, size_t totalObjects);

  void update(const Camera &camera, RenderQueue &queue, FrameContext &ctx);

  void renderImpostorMeshInstanced();
  void renderPointMeshInstanced();

  void addRenderSystem(RenderSystem *system);
  void addModelSource(ModelSource *object);
  void addUpdatable(Updatable *object);
  void addTrail(std::unique_ptr<Trail> trail);

  InstanceManager &getInstanceManager() { return this->instanceManager; };
  LODManager &getLODManager() { return this->lodManager; };
  Texture &getImpostorTexture() { return this->lodManager.getImpostorTexture(); };
  Buffer &getFullInstancesVBO() { return this->instanceManager.getFullInstancesVBO(); };
  std::vector<Trail *> &getTrails();
};