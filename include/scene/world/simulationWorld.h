#pragma once

#include "scene/world/IsimulationWorld.h"

#include "scene/world/data/sharedGPUBuffers.h"
#include "scene/world/data/sharedDatabase.h"
#include "scene/world/worldObject.h"

#include "render/world/renderWorld.h"

#include "physics/world/physicsWorld.h"
#include "physics/world/total.h"

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

class Planet;
class Moon;
class AsteroidSystem;
class ModelSource;
class ResourceManager;
class ThreadPool;
class Camera;
class Model;
struct Radii;
struct RotationalElements;
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

  std::vector<WorldObject> worldObjects;
  std::vector<WorldSystem> worldSystems;

  bool wasInit = false;

  void initDatabases(ResourceManager &resourceManager, ThreadPool &threadPool, double timeAfterJD2000);
  void initGPUBuffers(Context &ctx);

  void initRenderWorld(ResourceManager& manager, const FrameContext &ctx);

public:
  SimulationWorld();
  ~SimulationWorld() = default;

  void initCPU(ThreadPool &threadPool) override;
  void initGPU(ResourceManager &resourceManager) override;
  void init(RenderContext &renderCtx, ResourceManager &resourceManager, ThreadPool &threadPool, double startTime) override;

  void update(RenderQueue &queue, RenderContext &renderCtx) override;

  void addWorldObject(WorldObject object) { this->worldObjects.push_back(object); };
  void addWorldSystem(WorldSystem system) { this->worldSystems.push_back(system); };

  const IPhysicsWorld &getPhysicsWorld() const override { return this->physics; };
  RenderWorld &getRenderWorld() override { return this->render; };
};

#include "scene/world/simulationWorld.hpp"