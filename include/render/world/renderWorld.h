#pragma once

#include "render/lod/manager/lodManager.h"
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
struct FrameContext;
struct RenderDataGPU;
struct LODGPUData;

class RenderWorld
{
private:
  LODManager lodManager;
  InstanceManager instanceManager;

  std::vector<Updatable *> updatable;
  std::vector<RenderSystem *> renderSystems;
  std::vector<ModelSource *> modelSources;

  std::vector<std::unique_ptr<Trail>> trails;
  std::vector<Trail *> trailViews;

  CLBuffer instanceColorsBuffer;
  CLBuffer instanceTextureLayersBuffer;
  CLBuffer instanceImportancesBuffer;

  void buildQueue(const Camera &camera, RenderQueue &queue, FrameContext &ctx);
  void reserveModelInstances();
public:
  RenderWorld() = default;
  ~RenderWorld() = default;

  void init(size_t totalObjects);
  void initGPU(Context &ctx, RenderDataGPU &gpu);
  void initLODGPU(Context &ctx, LODGPUData &data, LODSettings &settings, size_t totalObjects);

  void initLODGPU();

  void update(const Camera &camera, RenderQueue &queue, FrameContext &ctx);

  void renderImpostorMeshInstanced();
  void renderPointMeshInstanced();

  void addRenderSystem(RenderSystem *system);
  void addModelSource(ModelSource *object);
  void addUpdatable(Updatable *object);
  void addTrail(std::unique_ptr<Trail> trail);

  Texture &getImpostorTexture() { return this->lodManager.getImpostorTexture(); };
  Buffer &getFullInstancesVBO() { return this->instanceManager.getFullInstancesVBO(); };
  std::vector<Trail *> &getTrails();
};