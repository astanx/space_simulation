#pragma once

#include "render/renderBatch.h"

#include <vector>

class Scene;
class LODManager;
class ModelSource;
class RenderQueueBuilder;
class InstanceManager;
struct FrameContext;
struct Frustum;

class RenderQueue
{
private:
  std::vector<RenderBatch> coreBatches;
  std::vector<RenderBatch> tangentBatches;
  std::vector<RenderBatch> shadowBatches;
  std::vector<RenderBatch> reflectorBatches;

  void buildModelSource(ModelSource *source, RenderQueueBuilder &builder, LODManager &lod, Frustum *frustum, InstanceManager &instance, FrameContext ctx, float fov);

  void clear();

public:
  RenderQueue() = default;
  ~RenderQueue() = default;

  void build(Scene &scene, LODManager &lod, InstanceManager &instance, FrameContext &ctx);

  void addCoreBatch(RenderBatch batch);
  void addTangentBatch(RenderBatch batch);
  void addShadowBatch(RenderBatch batch);
  void addReflectorBatch(RenderBatch batch);

  std::vector<RenderBatch> &getCoreBatches() { return this->coreBatches; };
  std::vector<RenderBatch> &getTangentBatches() { return this->tangentBatches; };
  std::vector<RenderBatch> &getShadowBatches() { return this->shadowBatches; };
  std::vector<RenderBatch> &getReflectorBatches() { return this->reflectorBatches; };
};