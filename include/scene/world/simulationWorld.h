#pragma once

#include "scene/world/IsimulationWorld.h"

#include "scene/world/data/sharedGPUBuffers.h"
#include "scene/world/data/sharedDatabase.h"
#include "scene/world/worldConfig.h"

#include "render/world/renderWorld.h"

#include "physics/world/physicsWorld.h"
#include "physics/world/total.h"
#include "physics/world/data/physicsDatabaseView.h"

#include "physics/structs/gravityField.h"
#include "physics/structs/hapkeParameters.h"
#include "physics/structs/tidalParameters.h"

#include "resources/entity/entityManager.h"

#include "compute/commandQueue.h"

struct Importance
{
  float base = 0;
  float asteroid = 0;
  float planet = 0;
  float moon = 0;
  float star = 0;
};

class ResourceManager;
class ThreadPool;
struct RenderContext;

template <typename Real>
class SimulationWorld : public ISimulationWorld
{
private:
  Importance importance;
  EntityManager entityManager;

  PhysicsWorld<Real> physics;
  RenderWorld render;

  SharedGPUBuffers gpu;
  SharedDatabase<Real> database;

  CommandQueue queue;

  bool wasInit = false;

  void initConfigs(WorldConfig &cfg);
  void initDatabases(ResourceManager &resourceManager, ThreadPool &threadPool, double timeAfterJD2000, bool enableRender);
  void initGPUBuffers(Context &ctx);

  void initRenderWorld(ResourceManager &manager, const FrameContext &ctx);

public:
  SimulationWorld();
  ~SimulationWorld() = default;

  void initCPU(ThreadPool &threadPool) override;
  void initGPU(ResourceManager &resourceManager) override;
  void init(RenderContext &renderCtx, ResourceManager &resourceManager, ThreadPool &threadPool, WorldConfig &cfg) override;

  void updatePhysics(double dt) override;
  void updateRender(RenderQueue &queue, RenderContext &renderCtx) override;

  const PhysicsDatabaseView<Real> getPhysicsWorldView() const;
  const EntityManager &getEntityManager() const { return this->entityManager; };

  const IPhysicsWorld &getPhysicsWorld() const override { return this->physics; };
  RenderWorld &getRenderWorld() override { return this->render; };
};

#include "scene/world/simulationWorld.hpp"