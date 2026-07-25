#pragma once

#include "scene/world/worldGPUData.h"

#include "render/renderWorld.h"

#include "physics/world/physicsWorld.h"

#include "physics/structs/gravityField.h"
#include "physics/structs/hapkeParameters.h"
#include "physics/structs/tidalParameters.h"

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
  Importance importance;

  PhysicsWorld physics;
  RenderWorld render;
  WorldGPUData gpu;

  Planet *createPlanet(Model &model, double mu, Radii radii, Object *centralBody, const KeplerElements keplerElements, const RotationalElements rotationalElements, double timeAfterJD2000, GravityField gravityField = GravityField(), TidalParameters tidalParameters = TidalParameters(), double g = 0.0);

  Star *createStar(Model &model, double mu, Radii radii, double luminosity, const RotationalElements rotationalElements, double timeAfterJD2000, glm::dvec3 position = glm::dvec3(0.0), glm::dvec3 velocity = glm::dvec3(0.0));

  Moon *createMoon(Model &model, double mu, Radii radii, Planet *centralBody, const KeplerElements &keplerElements, const RotationalElements rotationalElements, const HapkeParameters &hapkeParameters, double timeAfterJD2000, GravityField gravityField = GravityField(), TidalParameters tidalParameters = TidalParameters());

  void addAtmosphereToPlanet(ResourceManager &resourceManager, ThreadPool &threadPool, std::string planetName, Planet *planet);

  void addLayerToModelSource(Model &model, ModelSource *object);

  AsteroidSystem *createAsteroidSystem(ResourceManager &resourceManager, ThreadPool &threadPool, Object *centralBody, unsigned amount, double innerEdge, double outerEdge, double timeAfterJD2000);

  void initObjects(ResourceManager &resourceManager, ThreadPool &threadPool, double timeAfterJD2000);

public:
  SimulationWorld();
  ~SimulationWorld() = default;

  void init(ResourceManager &resourceManager, ThreadPool &threadPool, double startTime);

  void update(RenderContext &renderCtx, const Camera &camera);

  const PhysicsWorld &getPhysicsWorld() const { return this->physics; };
  RenderWorld &getRenderWorld() { return this->render; };
};