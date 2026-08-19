#pragma once

class ResourceManager;
class ThreadPool;
class RenderQueue;
class RenderWorld;
class IPhysicsWorld;
struct RenderContext;

class ISimulationWorld
{
public:
  virtual ~ISimulationWorld() = 0;

  virtual void initCPU() = 0;
  virtual void initGPU(ResourceManager &resourceManager) = 0;
  virtual void init(RenderContext &renderCtx, ResourceManager &resourceManager, ThreadPool &threadPool, double startTime) = 0;

  virtual void update(RenderQueue &queue, RenderContext &renderCtx) = 0;

  virtual const IPhysicsWorld &getPhysicsWorld() const = 0;
  virtual RenderWorld &getRenderWorld() = 0;
};