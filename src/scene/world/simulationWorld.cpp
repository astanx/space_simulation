#include "scene/world/simulationWorld.h"

#include "scene/world/worldGPUBuilder.h"

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

#include "physics/integrators/wisdomHolmanGPUData.h"

#include "graphics/primitives/ellipsoid.h"

#include "render/renderContext.h"

// Private functions
Planet *SimulationWorld::createPlanet(Model &model, double mu, Radii radii, Object *centralBody, const KeplerElements keplerElements, const RotationalElements rotationalElements, double timeAfterJD2000, GravityField gravityField, TidalParameters tidalParameters, double g)
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

Star *SimulationWorld::createStar(Model &model, double mu,
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

Moon *SimulationWorld::createMoon(Model &model, double mu,
                                  Radii radii, Planet *centralBody, const KeplerElements &keplerElements, const RotationalElements rotationalElements, const HapkeParameters &hapkeParameters, double timeAfterJD2000, GravityField gravityField, TidalParameters tidalParameters)
{
  KeplerElements e = keplerElements;
  e.calculateMeanMotion(centralBody->getMu());
  e.advanceMeanAnomaly(timeAfterJD2000);

  RotationalElements r = rotationalElements;
  r.advanceFromJD2000(timeAfterJD2000);

  std::unique_ptr<Moon> moon = std::make_unique<Moon>(centralBody, mu, radii, e, hapkeParameters, tidalParameters, gravityField);

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

AsteroidSystem *SimulationWorld::createAsteroidSystem(ResourceManager &resourceManager, ThreadPool &threadPool, Object *centralBody, unsigned amount, double innerEdge, double outerEdge, double timeAfterJD2000)
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

void SimulationWorld::addLayerToModelSource(Model &model, ModelSource *object)
{
  object->addLayer(&model);
}

void SimulationWorld::addAtmosphereToPlanet(ResourceManager &resourceManager, ThreadPool &threadPool, std::string planetName, Planet *planet)
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

void SimulationWorld::initObjects(ResourceManager &resourceManager, ThreadPool &threadPool, double timeAfterJD2000)
{
  Star *sunPtr = createStar(resourceManager.GetModel(Res::SUN_MODEL), sunMu, sunRadii, sunLuminosity, sunRotationalElements, timeAfterJD2000, sunPos);
  createPlanet(resourceManager.GetModel(Res::MERCURY_MODEL), mercuryMu, mercuryRadii, sunPtr, mercuryElements, mercuryRotationalElements, timeAfterJD2000);
  Planet *venusPtr = createPlanet(resourceManager.GetModel(Res::VENUS_MODEL), venusMu, venusRadii, sunPtr, venusElements, venusRotationalElements, timeAfterJD2000);
  addLayerToModelSource(resourceManager.GetModel(Res::VENUS_ATMOSPHERE_MODEL), venusPtr);
  Planet *earthPtr = createPlanet(resourceManager.GetModel(Res::EARTH_MODEL), earthMu, earthRadii, sunPtr, earthElements, earthRotationalElements, timeAfterJD2000, earthGravityField, earthTidalParameters, 9.80665); // temp
  addAtmosphereToPlanet(resourceManager, threadPool, Res::EARTH_MODEL, earthPtr);
  addLayerToModelSource(resourceManager.GetModel(Res::EARTH_ATMOSPHERE_MODEL), earthPtr);
  createMoon(resourceManager.GetModel(Res::MOON_MODEL), moonMu, moonRadii, earthPtr, moonElements, moonRotationalElements, moonHapkeParameters, timeAfterJD2000, moonGravityField, moonTidalParameters);
  createPlanet(resourceManager.GetModel(Res::MARS_MODEL), marsMu, marsRadii, sunPtr, marsElements, marsRotationalElements, timeAfterJD2000, marsGravityField);
  createAsteroidSystem(resourceManager, threadPool, sunPtr, 100, INNER_ASTEROID_BELT_EDGE, OUTER_ASTEROID_BELT_EDGE, timeAfterJD2000);
  createPlanet(resourceManager.GetModel(Res::JUPITER_MODEL), jupiterMu, jupiterRadii, sunPtr, jupiterElements, jupiterRotationalElements, timeAfterJD2000);

  this->physics.addSun(std::move(sunPtr));
}

template <typename Real>
void SimulationWorld::initGPUBuffers(Context &ctx, SharedDataGPU<Real> &data)
{
  this->gpu.positionsBuffer.init(ctx.get(), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, data.positions.size() * sizeof(Vec3<Real>), data.positions.data());
  this->gpu.orientationsBuffer.init(ctx.get(), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, data.orientations.size() * sizeof(Quat<Real>), data.orientations.data());
  this->gpu.meanRadiiBuffer.init(ctx.get(), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, data.meanRadii.size() * sizeof(Real), data.meanRadii.data());
  this->gpu.polarRadiiBuffer.init(ctx.get(), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, data.polarRadii.size() * sizeof(Real), data.polarRadii.data());
  this->gpu.equatorianRadiiBuffer.init(ctx.get(), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, data.equatorianRadii.size() * sizeof(Real), data.equatorianRadii.data());
}

void SimulationWorld::initGPU(ResourceManager &resourceManager)
{
  Context &ctx = resourceManager.GetContext(Res::MAIN_CONTEXT);

  this->queue.init(ctx.get(), ctx.getDevice());

  if (ctx.getSupportsDouble())
  {
    WorldGPUBuilder<double> builder;
    WorldDataGPU<double> data = builder.build(this->worldObjects, this->worldSystems, this->render.getInstanceManager());
    this->physics.initGPUBuffers<double>(ctx, data.physics);

    this->initGPUBuffers<double>(ctx, data.shared);

    WisdomHolmanGPUData integratorData{this->physics.getGPUData(), this->gpu};
    this->physics.initGPUIntegrator(resourceManager, ctx, integratorData, data.total);

    this->render.initGPU(ctx, data.render);
    this->render.initLODGPU(ctx, resourceManager, this->gpu, data.total.total);
  }
  else
  {
    WorldGPUBuilder<float> builder;
    WorldDataGPU<float> data = builder.build(this->worldObjects, this->worldSystems, this->render.getInstanceManager());
    this->physics.initGPUBuffers<float>(ctx, data.physics);

    this->initGPUBuffers<float>(ctx, data.shared);

    WisdomHolmanGPUData integratorData{this->physics.getGPUData(), this->gpu};
    this->physics.initGPUIntegrator(resourceManager, ctx, integratorData, data.total);

    this->render.initGPU(ctx, data.render);
    this->render.initLODGPU(ctx, resourceManager, this->gpu, data.total.total);
  }
}

// Constructor
SimulationWorld::SimulationWorld() : render(this->queue), physics(this->queue)
{
  this->importance.base = 1.f;
  this->importance.asteroid = 2.5f;
  this->importance.planet = 5.f;
  this->importance.moon = 3.f;
  this->importance.star = 12.f;
}

// Public fucntions
void SimulationWorld::init(ResourceManager &resourceManager, ThreadPool &threadPool, double startTime)
{
  double timeAfterJD2000 = startTime - JD_2000;
  timeAfterJD2000 *= 24 * 60 * 60; // Days to seconds
  this->initObjects(resourceManager, threadPool, timeAfterJD2000);
  // this->initGPU(resourceManager);

  this->render.init(total.total);
}

void SimulationWorld::update(const Camera &camera, RenderQueue &queue, RenderContext &renderCtx)
{
  if (!renderCtx.settings.paused)
    this->physics.step(renderCtx.deltaTime);

  this->render.update(camera, queue, renderCtx.frameCtx);
}