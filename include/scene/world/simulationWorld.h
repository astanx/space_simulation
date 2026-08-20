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
class Star;
class ModelSource;
class ResourceManager;
class ThreadPool;
class Camera;
class Model;
struct Radii;
struct KeplerElements;
struct RotationalElements;
struct RenderContext;

template <typename Real>
class SimulationWorld : public ISimulationWorld
{
private:
  Total total;
  Importance importance;

  PhysicsWorld<Real> physics;
  RenderWorld render;
  
  SharedGPUBuffers gpu;
  SharedDatabase<Real> database;

  CommandQueue queue;

  std::vector<WorldObject> worldObjects;
  std::vector<WorldSystem> worldSystems;

  bool wasInit = false;

  Planet *createPlanet(Model &model, double mu, Radii radii, Object *centralBody, const KeplerElements keplerElements, const RotationalElements rotationalElements, double timeAfterJD2000, GravityField gravityField = GravityField(), TidalParameters tidalParameters = TidalParameters(), double g = 0.0);
  Star *createStar(Model &model, double mu, Radii radii, double luminosity, const RotationalElements rotationalElements, double timeAfterJD2000, glm::dvec3 position = glm::dvec3(0.0), glm::dvec3 velocity = glm::dvec3(0.0));
  Moon *createMoon(Model &model, double mu, Radii radii, Planet *centralBody, const KeplerElements &keplerElements, const RotationalElements rotationalElements, double timeAfterJD2000, GravityField gravityField = GravityField(), TidalParameters tidalParameters = TidalParameters());
  void addAtmosphereToPlanet(ResourceManager &resourceManager, ThreadPool &threadPool, std::string planetName, Planet *planet);
  void addLayerToModelSource(Model &model, ModelSource *object);
  AsteroidSystem *createAsteroidSystem(ResourceManager &resourceManager, ThreadPool &threadPool, Object *centralBody, unsigned amount, double innerEdge, double outerEdge, double timeAfterJD2000);

  void initObjects(ResourceManager &resourceManager, ThreadPool &threadPool, double timeAfterJD2000);
  void initGPUBuffers(Context &ctx, SharedDatabase<Real> &data);

public:
  SimulationWorld();
  ~SimulationWorld() = default;

  void initCPU() override;
  void initGPU(ResourceManager &resourceManager) override;
  void init(RenderContext &renderCtx, ResourceManager &resourceManager, ThreadPool &threadPool, double startTime) override;

  void update(RenderQueue &queue, RenderContext &renderCtx) override;

  void addWorldObject(WorldObject object) { this->worldObjects.push_back(object); };
  void addWorldSystem(WorldSystem system) { this->worldSystems.push_back(system); };

  const IPhysicsWorld &getPhysicsWorld() const override { return this->physics; };
  RenderWorld &getRenderWorld() override { return this->render; };
};

#include "scene/world/simulationWorld.hpp"