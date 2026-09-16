#pragma once

class ResourceManager;
class ThreadPool;
class RenderQueue;
class RenderWorld;
class IPhysicsWorld;
struct RenderContext;
struct WorldConfig;

class ISimulationWorld
{
public:
  virtual ~ISimulationWorld() = 0;

  virtual void initCPU(ThreadPool &threadPool) = 0;
  virtual void initGPU(ResourceManager &resourceManager) = 0;
  virtual void init(RenderContext &renderCtx, ResourceManager &resourceManager, ThreadPool &threadPool, WorldConfig &cfg) = 0;

  virtual void updatePhysics(double dt) = 0;
  virtual void updateRender(RenderQueue &queue, RenderContext &renderCtx) = 0;

  virtual const IPhysicsWorld &getPhysicsWorld() const = 0;
  virtual RenderWorld &getRenderWorld() = 0;
};