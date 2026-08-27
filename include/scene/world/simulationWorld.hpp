#pragma once

#include "scene/world/simulationWorld.h"

#include "scene/world/database/worldDatabaseBuilder.h"

#include "resources/resourceManager.h"
#include "resources/resources.h"

#include "maths/constants.h"

#include "physics/constants.h"
#include "physics/object.h"
#include "physics/planet.h"
#include "physics/systems/asteroidSystem.h"

#include "physics/structs/radii.h"
#include "physics/structs/keplerElements.h"
#include "physics/structs/rotationalElements.h"

#include "physics/integrators/data/wisdomHolmanGPUBuffers.h"

#include "graphics/primitives/ellipsoid.h"
#include "graphics/skybox.h"

#include "render/renderContext.h"

// Private functions
template <typename Real>
void SimulationWorld<Real>::initDatabases(ResourceManager &resourceManager, ThreadPool &threadPool, double timeAfterJD2000)
{
  WorldDatabaseBuilder<Real> builder(this->entityManager, this->importance);

  Object *sunPtr = builder.createStar(resourceManager.GetModel(Res::SUN_MODEL), sunMu, sunRadii, sunLuminosity, sunRotationalElements, timeAfterJD2000, sunPos);
  builder.createPlanet(resourceManager.GetModel(Res::MERCURY_MODEL), mercuryMu, mercuryRadii, sunPtr, mercuryElements, mercuryRotationalElements, timeAfterJD2000);
  Planet *venusPtr = builder.createPlanet(resourceManager.GetModel(Res::VENUS_MODEL), venusMu, venusRadii, sunPtr, venusElements, venusRotationalElements, timeAfterJD2000);
  Planet *earthPtr = builder.createPlanet(resourceManager.GetModel(Res::EARTH_MODEL), earthMu, earthRadii, sunPtr, earthElements, earthRotationalElements, timeAfterJD2000, earthGravityField, earthTidalParameters, 9.80665); // temp
  glm::dmat3 tensor = earthPtr->getInertiaTensor();

  builder.addAtmosphereToPlanet(resourceManager, threadPool, Res::EARTH_MODEL, earthPtr);
  builder.createMoon(resourceManager.GetModel(Res::MOON_MODEL), moonMu, moonRadii, earthPtr, moonElements, moonRotationalElements, timeAfterJD2000, moonGravityField, moonTidalParameters);
  builder.createPlanet(resourceManager.GetModel(Res::MARS_MODEL), marsMu, marsRadii, sunPtr, marsElements, marsRotationalElements, timeAfterJD2000, marsGravityField);
  builder.createAsteroidSystem(resourceManager, threadPool, sunPtr, 100, INNER_ASTEROID_BELT_EDGE, OUTER_ASTEROID_BELT_EDGE, timeAfterJD2000);
  builder.createPlanet(resourceManager.GetModel(Res::JUPITER_MODEL), jupiterMu, jupiterRadii, sunPtr, jupiterElements, jupiterRotationalElements, timeAfterJD2000);

  WorldDatabase<Real> data = builder.build(this->render.getInstanceManager());

  this->database = data.shared;
  this->physics.setDatabase(data.physics);
  this->render.setDatabase(data.render);

  this->physics.addSun(builder.convertObjectToEntity(sunPtr));
}

template <typename Real>
void SimulationWorld<Real>::initGPUBuffers(Context &ctx)
{
  this->gpu.positionsBuffer.init(ctx.get(), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, this->database.positions.size() * sizeof(Vec3<Real>), this->database.positions.data());
  this->gpu.orientationsBuffer.init(ctx.get(), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, this->database.orientations.size() * sizeof(Quat<Real>), this->database.orientations.data());
  this->gpu.meanRadiiBuffer.init(ctx.get(), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, this->database.meanRadii.size() * sizeof(Real), this->database.meanRadii.data());
  this->gpu.polarRadiiBuffer.init(ctx.get(), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, this->database.polarRadii.size() * sizeof(Real), this->database.polarRadii.data());
  this->gpu.equatorianRadiiBuffer.init(ctx.get(), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, this->database.equatorianRadii.size() * sizeof(Real), this->database.equatorianRadii.data());
}
template <typename Real>
void SimulationWorld<Real>::initRenderWorld(ResourceManager &manager, const FrameContext &ctx)
{
  this->render.init(this->database.total);
  const Entity &sun = this->physics.getSun();
  size_t sunIdx = this->entityManager.getObjectIndex(sun);
  this->render.addPointLight(std::make_unique<PointLight>(this->database.positions[sunIdx], glm::vec3(1.0f), this->database.luminosities[sunIdx], this->database.meanRadii[sunIdx]));
  this->render.addCamera(std::make_unique<Camera>(this->database.positions[sunIdx], glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), ctx.width, ctx.height));
  this->render.addSkybox(std::make_unique<Skybox>("assets/skybox/starmap.exr", manager));
}

// Constructor
template <typename Real>
SimulationWorld<Real>::SimulationWorld() : queue()
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

  this->physics.initGPUBuffers(ctx);

  this->initGPUBuffers(ctx);

  WisdomHolmanGPUBuffers integratorBuffers{this->physics.getGPUBuffers(), this->gpu};
  this->physics.initGPUBackend(resourceManager, ctx, this->queue, integratorBuffers, this->database.total);

  this->render.initGPUBuffers(ctx);
  this->render.initGPUBackend(ctx, this->queue, this->database.total, resourceManager, this->gpu);
}

template <typename Real>
void SimulationWorld<Real>::initCPU(ThreadPool &threadPool)
{
  if (!this->wasInit)
    Logger::logFatal("Simulation World", "Backend should be initialized after world");

  this->physics.initCPUBackend(this->entityManager, this->database, threadPool);
  this->render.initCPUBackend();
}

template <typename Real>
void SimulationWorld<Real>::init(RenderContext &ctx, ResourceManager &resourceManager, ThreadPool &threadPool, double startTime)
{
  if (this->wasInit)
    Logger::logWarning("SimulationWorld", "World initialized twice");

  double timeAfterJD2000 = startTime - JD_2000;
  timeAfterJD2000 *= 24 * 60 * 60; // Days to seconds
  this->initDatabases(resourceManager, threadPool, timeAfterJD2000);

  this->initRenderWorld(resourceManager, ctx.frameCtx);

  this->wasInit = true;
}

template <typename Real>
void SimulationWorld<Real>::update(RenderQueue &queue, RenderContext &renderCtx)
{
  if (!renderCtx.settings.paused)
    this->physics.step(renderCtx.deltaTime);

  this->render.update(queue, renderCtx.frameCtx, this->database, this->entityManager);

  this->render.sync(this->physics, this->database, this->entityManager);
}