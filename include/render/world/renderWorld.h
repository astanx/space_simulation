#pragma once

#include "render/lod/manager/lodManager.h"
#include "render/lod/manager/lodManagerGPU.h"
#include "render/instanceManager.h"
#include "render/queue/builder/renderQueueBuilderGPU.h"

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
  std::unique_ptr<RenderQueueBuilderGPU> renderQueueBuilderGPU;
  LODSettings lodSettings;

  InstanceManager instanceManager;

  std::vector<Updatable *> updatable;
  std::vector<RenderSystem *> renderSystems;
  std::vector<ModelSource *> modelSources;

  std::vector<std::unique_ptr<Trail>> trails;
  std::vector<Trail *> trailViews;

  CommandQueue &queue;
  Total &total;

  CLBuffer instanceColorsBuffer;
  CLBuffer instanceTextureLayersBuffer;
  CLBuffer instanceImportancesBuffer;
  CLBuffer modelRangeStartBuffer;
  CLBuffer modelRangeEndBuffer;
  CLBuffer modelFullCountBuffer;
  uint32_t rangeCount;

  std::vector<Model *> models;

  void buildQueue(const Camera &camera, RenderQueue &queue, FrameContext &ctx);
  void reserveModelInstances();

public:
  RenderWorld(CommandQueue &queue, Total &total) : queue(queue), lodManager(lodSettings), total(total) {};
  ~RenderWorld() = default;

  void init();
  void initGPU(Context &ctx, RenderDataGPU &gpu);
  void initRenderQueueGPU(Context &ctx, ResourceManager &resourceManager, SharedGPUData &data);

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