#pragma once

#include "scene/world/simulationWorld.h"

#include "scene/world/database/worldDatabaseBuilder.h"

#include "resources/resourceManager.h"
#include "resources/resources.h"

#include "maths/constants.h"

#include "physics/constants.h"
#include "physics/object.h"
#include "physics/planet.h"
#include "physics/star.h"
#include "physics/systems/asteroidSystem.h"

#include "physics/structs/radii.h"
#include "physics/structs/keplerElements.h"
#include "physics/structs/rotationalElements.h"

#include "physics/integrators/wisdomHolmanGPUBuffers.h"

#include "graphics/primitives/ellipsoid.h"

#include "render/renderContext.h"

// Private functions
template <typename Real>
Planet *SimulationWorld<Real>::createPlanet(Model &model, double mu, Radii radii, Object *centralBody, const KeplerElements keplerElements, const RotationalElements rotationalElements, double timeAfterJD2000, GravityField gravityField, TidalParameters tidalParameters, double g)
{
  KeplerElements e = keplerElements;
  e.calculateMeanMotion(centralBody->getMu());
  e.advanceMeanAnomaly(timeAfterJD2000);

  RotationalElements r = rotationalElements;
  r.advanceFromJD2000(timeAfterJD2000);

  std::unique_ptr<Planet> planet = std::make_unique<Planet>(centralBody, mu, radii, e, tidalParameters, gravityField, g);

  planet->setAngularVelocity(r.calculateAngularVelocity());
  planet->setOrientation(r.calculateOrientation());

  model.setImportance(this->importance.planet);
  planet->addMainLayer(&model);

  Planet *ptr = planet.get();

  if (planet->getUseTrail())
    this->render.addTrail(planet->generateTrail());

  this->addWorldObject({ptr, ptr});
  this->render.addModelSource(ptr);
  this->render.addUpdatable(ptr);
  this->physics.addObject(ptr);
  this->physics.addIntegratable(ptr);
  this->physics.addPlanetarObject(std::move(planet));
  this->total.orbital++;
  this->total.total++;

  return ptr;
}

template <typename Real>
Star *SimulationWorld<Real>::createStar(Model &model, double mu,
                                        Radii radii, double luminosity, const RotationalElements rotationalElements, double timeAfterJD2000, glm::dvec3 position, glm::dvec3 velocity)
{
  RotationalElements r = rotationalElements;
  r.advanceFromJD2000(timeAfterJD2000);

  std::unique_ptr<Star> star = std::make_unique<Star>(mu, radii, luminosity, position, velocity);

  star->setAngularVelocity(r.calculateAngularVelocity());
  star->setOrientation(r.calculateOrientation());

  model.setImportance(this->importance.star);
  star->addMainLayer(&model);

  Star *ptr = star.get();

  this->addWorldObject({ptr, ptr});
  this->render.addModelSource(ptr);
  this->render.addUpdatable(ptr);
  this->physics.addObject(ptr);
  this->physics.addIntegratable(ptr);
  this->physics.addStar(std::move(star));
  this->total.object++;
  this->total.total++;

  return ptr;
}

template <typename Real>
Moon *SimulationWorld<Real>::createMoon(Model &model, double mu, Radii radii, Planet *centralBody, const KeplerElements &keplerElements, const RotationalElements rotationalElements, double timeAfterJD2000, GravityField gravityField, TidalParameters tidalParameters)
{
  KeplerElements e = keplerElements;
  e.calculateMeanMotion(centralBody->getMu());
  e.advanceMeanAnomaly(timeAfterJD2000);

  RotationalElements r = rotationalElements;
  r.advanceFromJD2000(timeAfterJD2000);

  std::unique_ptr<Moon> moon = std::make_unique<Moon>(centralBody, mu, radii, e, tidalParameters, gravityField);

  moon->setAngularVelocity(r.calculateAngularVelocity());
  moon->setOrientation(r.calculateOrientation());

  model.setImportance(this->importance.moon);

  moon->addMainLayer(&model);
  if (moon->getUseTrail())
    this->render.addTrail(moon->generateTrail());

  Moon *ptr = moon.get();

  assert(centralBody && "[SimulationWorld] ASSERT: No central body for moon");

  this->addWorldObject({ptr, ptr});
  this->render.addUpdatable(ptr);
  this->render.addModelSource(ptr);
  this->physics.addObject(ptr);
  this->physics.addIntegratable(ptr);
  centralBody->addMoon(std::move(moon));
  this->total.orbital++;
  this->total.total++;

  return ptr;
}

template <typename Real>
AsteroidSystem *SimulationWorld<Real>::createAsteroidSystem(ResourceManager &resourceManager, ThreadPool &threadPool, Object *centralBody, unsigned amount, double innerEdge, double outerEdge, double timeAfterJD2000)
{
  std::unique_ptr<AsteroidSystem> system = std::make_unique<AsteroidSystem>(resourceManager, centralBody, amount,
                                                                            innerEdge, outerEdge,
                                                                            timeAfterJD2000, this->importance.asteroid, threadPool);
  AsteroidSystem *ptr = system.get();

  this->total.orbital += system->getTotalObjects();
  this->total.total += system->getTotalObjects();

  this->addWorldSystem({ptr, ptr});
  this->render.addRenderSystem(ptr);
  this->physics.addSystem(ptr);
  this->physics.addIntegratable(ptr);
  this->physics.addAsteroidSystem(std::move(system));

  return ptr;
}

template <typename Real>
void SimulationWorld<Real>::addLayerToModelSource(Model &model, ModelSource *object)
{
  object->addLayer(&model);
}
template <typename Real>
void SimulationWorld<Real>::addAtmosphereToPlanet(ResourceManager &resourceManager, ThreadPool &threadPool, std::string planetName, Planet *planet)
{
  std::string path = "assets/data/" + planetName + "/atmosphere/32_resolution";
  std::unique_ptr atmosphere = std::make_unique<Atmosphere>(planet, path, threadPool);
  Atmosphere *ptr = atmosphere.get();

  std::unique_ptr<Ellipsoid> obj = std::make_unique<Ellipsoid>(32, atmosphere->getRadii());
  resourceManager.LoadMesh<VertexPositionTexcoordNormal>(path, std::move(obj), VertexLayout::NoColor);
  Mesh &mesh = resourceManager.GetMesh(path);
  std::unique_ptr<Model> model = std::make_unique<Model>(mesh);

  this->physics.addAtmosphere(ptr);
  // planet->addLayer(std::move(model));
  planet->addAtmosphere(std::move(atmosphere));
}

template <typename Real>
void SimulationWorld<Real>::initObjects(ResourceManager &resourceManager, ThreadPool &threadPool, double timeAfterJD2000)
{
  Star *sunPtr = createStar(resourceManager.GetModel(Res::SUN_MODEL), sunMu, sunRadii, sunLuminosity, sunRotationalElements, timeAfterJD2000, sunPos);
  createPlanet(resourceManager.GetModel(Res::MERCURY_MODEL), mercuryMu, mercuryRadii, sunPtr, mercuryElements, mercuryRotationalElements, timeAfterJD2000);
  Planet *venusPtr = createPlanet(resourceManager.GetModel(Res::VENUS_MODEL), venusMu, venusRadii, sunPtr, venusElements, venusRotationalElements, timeAfterJD2000);
  addLayerToModelSource(resourceManager.GetModel(Res::VENUS_ATMOSPHERE_MODEL), venusPtr);
  Planet *earthPtr = createPlanet(resourceManager.GetModel(Res::EARTH_MODEL), earthMu, earthRadii, sunPtr, earthElements, earthRotationalElements, timeAfterJD2000, earthGravityField, earthTidalParameters, 9.80665); // temp
  addAtmosphereToPlanet(resourceManager, threadPool, Res::EARTH_MODEL, earthPtr);
  addLayerToModelSource(resourceManager.GetModel(Res::EARTH_ATMOSPHERE_MODEL), earthPtr);
  createMoon(resourceManager.GetModel(Res::MOON_MODEL), moonMu, moonRadii, earthPtr, moonElements, moonRotationalElements, timeAfterJD2000, moonGravityField, moonTidalParameters);
  createPlanet(resourceManager.GetModel(Res::MARS_MODEL), marsMu, marsRadii, sunPtr, marsElements, marsRotationalElements, timeAfterJD2000, marsGravityField);
  createAsteroidSystem(resourceManager, threadPool, sunPtr, 100, INNER_ASTEROID_BELT_EDGE, OUTER_ASTEROID_BELT_EDGE, timeAfterJD2000);
  Planet *jupiter = createPlanet(resourceManager.GetModel(Res::JUPITER_MODEL), jupiterMu, jupiterRadii, sunPtr, jupiterElements, jupiterRotationalElements, timeAfterJD2000);

  this->physics.addSun(sunPtr);
}

template <typename Real>
void SimulationWorld<Real>::initGPUBuffers(Context &ctx, SharedDatabase<Real> &data)
{
  this->gpu.positionsBuffer.init(ctx.get(), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, data.positions.size() * sizeof(Vec3<Real>), data.positions.data());
  this->gpu.orientationsBuffer.init(ctx.get(), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, data.orientations.size() * sizeof(Quat<Real>), data.orientations.data());
  this->gpu.meanRadiiBuffer.init(ctx.get(), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, data.meanRadii.size() * sizeof(Real), data.meanRadii.data());
  this->gpu.polarRadiiBuffer.init(ctx.get(), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, data.polarRadii.size() * sizeof(Real), data.polarRadii.data());
  this->gpu.equatorianRadiiBuffer.init(ctx.get(), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, data.equatorianRadii.size() * sizeof(Real), data.equatorianRadii.data());
}

// Constructor
template <typename Real>
SimulationWorld<Real>::SimulationWorld() : total(), queue()
{
  this->importance.base = 1.f;
  this->importance.asteroid = 2.5f;
  this->importance.planet = 5.f;
  this->importance.moon = 3.f;
  this->importance.star = 12.f;
}

// Public fucntions
template <typename Real>
void SimulationWorld<Real>::initGPU(ResourceManager &resourceManager)
{
  if (!this->wasInit)
    Logger::logFatal("Simulation World", "Backend should be initialized after world");

  Context &ctx = resourceManager.GetContext(Res::MAIN_CONTEXT);

  this->queue.init(ctx.get(), ctx.getDevice());

  WorldDatabaseBuilder<Real> builder;
  WorldDatabase<Real> data = builder.build(this->worldObjects, this->worldSystems, this->render.getInstanceManager());
  this->physics.initGPUBuffers(ctx, data.physics);

  this->initGPUBuffers(ctx, data.shared);

  WisdomHolmanGPUBuffers integratorBuffers{this->physics.getGPUBuffers(), this->gpu};
  this->physics.initGPUBackend(resourceManager, ctx, this->queue, integratorBuffers, this->total);

  this->render.initGPUBuffers(ctx, data.render);
  this->render.initGPUBackend(ctx, this->queue, this->total, resourceManager, this->gpu);
}

template <typename Real>
void SimulationWorld<Real>::initCPU()
{
  if (!this->wasInit)
    Logger::logFatal("Simulation World", "Backend should be initialized after world");

  this->physics.initCPUBackend();
  this->render.initCPUBackend();
}

template <typename Real>
void SimulationWorld<Real>::init(RenderContext &ctx, ResourceManager &resourceManager, ThreadPool &threadPool, double startTime)
{
  if (this->wasInit)
    Logger::logWarning("SimulationWorld", "World initialized twice");

  double timeAfterJD2000 = startTime - JD_2000;
  timeAfterJD2000 *= 24 * 60 * 60; // Days to seconds
  this->initObjects(resourceManager, threadPool, timeAfterJD2000);

  this->render.init(resourceManager, this->physics, ctx, this->total);

  this->wasInit = true;
}

template <typename Real>
void SimulationWorld<Real>::update(RenderQueue &queue, RenderContext &renderCtx)
{
  if (!renderCtx.settings.paused)
    this->physics.step(renderCtx.deltaTime);

  this->render.update(queue, renderCtx.frameCtx);

  this->render.sync(this->physics);
}