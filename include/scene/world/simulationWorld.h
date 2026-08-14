#pragma once

#include "scene/world/data/sharedGPUData.h"
#include "scene/world/data/sharedDataGPU.h"
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
  float base;
  float asteroid;
  float planet;
  float moon;
  float star;
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

class SimulationWorld
{
private:
  Total total;
  Importance importance;

  PhysicsWorld physics;
  RenderWorld render;
  SharedGPUData gpu;

  CommandQueue queue;

  std::vector<WorldObject> worldObjects;
  std::vector<WorldSystem> worldSystems;

  Planet *createPlanet(Model &model, double mu, Radii radii, Object *centralBody, const KeplerElements keplerElements, const RotationalElements rotationalElements, double timeAfterJD2000, GravityField gravityField = GravityField(), TidalParameters tidalParameters = TidalParameters(), double g = 0.0);
  Star *createStar(Model &model, double mu, Radii radii, double luminosity, const RotationalElements rotationalElements, double timeAfterJD2000, glm::dvec3 position = glm::dvec3(0.0), glm::dvec3 velocity = glm::dvec3(0.0));
  Moon *createMoon(Model &model, double mu, Radii radii, Planet *centralBody, const KeplerElements &keplerElements, const RotationalElements rotationalElements, double timeAfterJD2000, GravityField gravityField = GravityField(), TidalParameters tidalParameters = TidalParameters());
  void addAtmosphereToPlanet(ResourceManager &resourceManager, ThreadPool &threadPool, std::string planetName, Planet *planet);
  void addLayerToModelSource(Model &model, ModelSource *object);
  AsteroidSystem *createAsteroidSystem(ResourceManager &resourceManager, ThreadPool &threadPool, Object *centralBody, unsigned amount, double innerEdge, double outerEdge, double timeAfterJD2000);

  void initObjects(ResourceManager &resourceManager, ThreadPool &threadPool, double timeAfterJD2000);
  void initCPU();
  void initGPU(ResourceManager &resourceManager);
  template <typename Real>
  void initGPUBuffers(Context &ctx, SharedDataGPU<Real> &data);

public:
  SimulationWorld();
  ~SimulationWorld() = default;

  void init(ResourceManager &resourceManager, ThreadPool &threadPool, double startTime);

  void update(const Camera &camera, RenderQueue &queue, RenderContext &renderCtx);

  void addWorldObject(WorldObject object) { this->worldObjects.push_back(object); };
  void addWorldSystem(WorldSystem system) { this->worldSystems.push_back(system); };

  const PhysicsWorld &getPhysicsWorld() const { return this->physics; };
  RenderWorld &getRenderWorld() { return this->render; };
};