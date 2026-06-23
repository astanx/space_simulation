#include "scene/scene.h"

#include "debug/logger.h"

#include "graphics/shader.h"
#include "graphics/mesh.h"

#include "physics/planet.h"
#include "physics/orbit.h"
#include "physics/star.h"
#include "physics/moon.h"
#include "physics/constants.h"

#include "physics/systems/asteroidSystem.h"

#include "resources/resources.h"
#include "resources/resourceManager.h"

#include "render/updatable.h"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

// Private functions
Planet *Scene::createPlanet(std::string name, std::string material_name, double mu,
                            Radii radii, Object *centralBody, const KeplerElements keplerElements, const RotationalElements rotationalElements, double timeAfterJD2000, GravityField gravityField, TidalParameters tidalParameters, double g)
{
  Mesh &mesh = this->resourceManager.GetMesh(name);
  Material &mat = this->resourceManager.GetMaterial(material_name);
  std::unique_ptr<Model> model = std::make_unique<Model>(glm::dvec3(0.0), mat, mesh);

  KeplerElements e = keplerElements;
  e.calculateMeanMotion(centralBody->getMu());
  e.advanceMeanAnomaly(timeAfterJD2000);

  RotationalElements r = rotationalElements;
  r.advanceFromJD2000(timeAfterJD2000);

  std::unique_ptr<Planet> planet = std::make_unique<Planet>(centralBody, mu, radii, e, tidalParameters, gravityField, g);

  planet->setAngularVelocity(r.calculateAngularVelocity());
  planet->setOrientation(r.calculateOrientation());

  planet->addMainLayer(std::move(model));

  Planet *ptr = planet.get();

  if (planet->getUseTrail())
    this->addTrail(planet->generateTrail());

  this->addRenderable(ptr);
  this->addUpdatable(ptr);
  this->physicsWorld.addObject(ptr);
  this->physicsWorld.addIntegratableObject(ptr);
  this->physicsWorld.addPlanetarObject(std::move(planet));

  return ptr;
}

Star *Scene::createStar(std::string name, std::string material_name, double mu,
                        Radii radii, double luminosity, const RotationalElements rotationalElements, double timeAfterJD2000, glm::dvec3 position, glm::dvec3 velocity)
{
  Mesh &mesh = this->resourceManager.GetMesh(name);
  Material &mat = this->resourceManager.GetMaterial(material_name);
  std::unique_ptr<Model> model = std::make_unique<Model>(glm::dvec3(0.0), mat, mesh);

  RotationalElements r = rotationalElements;
  r.advanceFromJD2000(timeAfterJD2000);

  std::unique_ptr<Star> star = std::make_unique<Star>(mu, radii, luminosity, position, velocity);

  star->setAngularVelocity(r.calculateAngularVelocity());
  star->setOrientation(r.calculateOrientation());

  star->addMainLayer(std::move(model));

  Star *ptr = star.get();
  this->addRenderable(ptr);
  this->addUpdatable(ptr);
  this->physicsWorld.addObject(ptr);
  this->physicsWorld.addIntegratableObject(ptr);
  this->physicsWorld.addStar(std::move(star));

  return ptr;
}

Moon *Scene::createMoon(std::string name, std::string material_name, double mu,
                        Radii radii, Planet *centralBody, const KeplerElements &keplerElements, const RotationalElements rotationalElements, const HapkeParameters &hapkeParameters, double timeAfterJD2000, GravityField gravityField, TidalParameters tidalParameters)
{
  Mesh &mesh = this->resourceManager.GetMesh(name);
  Material &mat = this->resourceManager.GetMaterial(material_name);
  std::unique_ptr<Model> model = std::make_unique<Model>(glm::dvec3(0.0), mat, mesh);

  KeplerElements e = keplerElements;
  e.calculateMeanMotion(centralBody->getMu());
  e.advanceMeanAnomaly(timeAfterJD2000);

  RotationalElements r = rotationalElements;
  r.advanceFromJD2000(timeAfterJD2000);

  std::unique_ptr<Moon> moon = std::make_unique<Moon>(centralBody, mu, radii, e, hapkeParameters, tidalParameters, gravityField);

  moon->setAngularVelocity(r.calculateAngularVelocity());
  moon->setOrientation(r.calculateOrientation());

  moon->addMainLayer(std::move(model));
  if (moon->getUseTrail())
    this->addTrail(moon->generateTrail());

  Moon *ptr = moon.get();

  assert(centralBody && "[Scene] ASSERT: No central body for moon");

  this->addUpdatable(ptr);
  this->addRenderable(ptr);
  this->physicsWorld.addObject(ptr);
  this->physicsWorld.addIntegratableObject(ptr);
  centralBody->addMoon(std::move(moon));

  return ptr;
}

AsteroidSystem *Scene::createAsteroidSystem(Object *centralBody, unsigned amount, double innerEdge, double outerEdge, double timeAfterJD2000)
{
  std::unique_ptr<AsteroidSystem> system = std::make_unique<AsteroidSystem>(centralBody, amount,
                                                                            innerEdge, outerEdge,
                                                                            timeAfterJD2000,
                                                                            &this->resourceManager.GetMaterial(Res::ASTEROID_MATERIAL), this->threadPool);
  AsteroidSystem *ptr = system.get();
  this->addUpdatable(ptr);
  this->physicsWorld.addIntegratableObject(ptr);
  this->physicsWorld.addAsteroidSystem(std::move(system));

  return ptr;
}

void Scene::addLayerToModelSource(std::string name, std::string material_name, ModelSource *object)
{
  Mesh &mesh = this->resourceManager.GetMesh(name);
  Material &mat = this->resourceManager.GetMaterial(material_name);
  std::unique_ptr<Model> model = std::make_unique<Model>(glm::dvec3(0.0), mat, mesh);

  object->addLayer(std::move(model));
}

void Scene::addAtmosphereToPlanet(std::string planetName, Planet *planet)
{
  std::string path = "assets/data/" + planetName + "/atmosphere/32_resolution";
  std::unique_ptr atmosphere = std::make_unique<Atmosphere>(path, this->threadPool, planet->getFreeFallAcc(), planet->getRadius());
  planet->addAtmosphere(std::move(atmosphere));
}

// Constructor/Destructor
Scene::Scene(ResourceManager &resourceManager, ThreadPool &threadPool) : threadPool(threadPool), resourceManager(resourceManager)
{
  this->activeCamera = nullptr;
  this->skybox = nullptr;
}
Scene::~Scene() = default;

// Process functions
void Scene::init(RenderContext &renderCtx, double startTime)
{
  double timeAfterJD2000 = startTime - JD_2000;
  timeAfterJD2000 *= 24 * 60 * 60; // Days to seconds

  Star *sunPtr = createStar(Res::SUN, Res::SUN_MATERIAL, sunMu, sunRadii, sunLuminosity, sunRotationalElements, timeAfterJD2000, sunPos);
  createPlanet(Res::MERCURY, Res::MERCURY_MATERIAL, mercuryMu, mercuryRadii, sunPtr, mercuryElements, mercuryRotationalElements, timeAfterJD2000);
  Planet *venusPtr = createPlanet(Res::VENUS, Res::VENUS_MATERIAL, venusMu, venusRadii, sunPtr, venusElements, venusRotationalElements, timeAfterJD2000);
  addLayerToModelSource(Res::VENUS_ATMOSPHERE, Res::VENUS_ATMOSPHERE_MATERIAL, venusPtr);
  Planet *earthPtr = createPlanet(Res::EARTH, Res::EARTH_MATERIAL, earthMu, earthRadii, sunPtr, earthElements, earthRotationalElements, timeAfterJD2000, earthGravityField, earthTidalParameters, 9.80665); // temp
  addAtmosphereToPlanet(Res::EARTH, earthPtr);
  addLayerToModelSource(Res::EARTH_ATMOSPHERE, Res::EARTH_ATMOSPHERE_MATERIAL, earthPtr);
  createMoon(Res::MOON, Res::MOON_MATERIAL, moonMu, moonRadii, earthPtr, moonElements, moonRotationalElements, moonHapkeParameters, timeAfterJD2000, moonGravityField, moonTidalParameters);
  createPlanet(Res::MARS, Res::MARS_MATERIAL, marsMu, marsRadii, sunPtr, marsElements, marsRotationalElements, timeAfterJD2000, marsGravityField);
  createAsteroidSystem(sunPtr, 100, INNER_ASTEROID_BELT_EDGE, OUTER_ASTEROID_BELT_EDGE, timeAfterJD2000);
  createPlanet(Res::JUPITER, Res::JUPITER_MATERIAL, jupiterMu, jupiterRadii, sunPtr, jupiterElements, jupiterRotationalElements, timeAfterJD2000);

  std::unique_ptr<PointLight> pointLight = std::make_unique<PointLight>(
      sunPtr->getRenderPosition(),
      glm::vec3(1.0f),
      sunPtr->getLuminosity(),
      sunPtr->getRadius());
  this->addPointLight(std::move(pointLight));

  this->physicsWorld.addSun(std::move(sunPtr));

  std::unique_ptr<Camera> cam = std::make_unique<Camera>(sunPos,
                                                         glm::vec3(0.0f, 0.0f, -1.0f),
                                                         glm::vec3(0.0f, 1.0f, 0.0f),
                                                         renderCtx.frameCtx.width, renderCtx.frameCtx.height);
  this->addCamera(std::move(cam));
  activeCamera = this->cameras.back().get();

  std::unique_ptr<Skybox> sb = std::make_unique<Skybox>("assets/skybox/starmap.exr", this->resourceManager);
  this->addSkybox(std::move(sb));
  this->skybox = this->skyboxes.back().get();
}

void Scene::processKeyboard(CameraMovement direction, float deltaTime)
{
  assert(this->activeCamera && "[Scene] ASSERT: No active camera to process keyboard");

  this->activeCamera->processKeyboard(direction, deltaTime);
}

void Scene::processMouseMovement(const float &xpos, const float &ypos)
{
  assert(this->activeCamera && "[Scene] ASSERT: No active camera to process mouse movement");

  this->activeCamera->processMouseMovement(xpos, ypos);
}

void Scene::processMouseScroll(float yoffset)
{
  assert(this->activeCamera && "[Scene] ASSERT: No active camera to process mouse scroll");

  this->activeCamera->processMouseScroll(yoffset);
}

void Scene::update(RenderContext &renderCtx)
{
  if (!renderCtx.settings.paused)
    this->physicsWorld.step(renderCtx.deltaTime);

  for (Updatable *&object : this->updatable)
    object->update(this->getActiveCamera());

  for (std::unique_ptr<Trail> &trail : this->trails)
    trail->update(this->getActiveCamera());

  if (this->pointLights[0])
    this->pointLights[0]->move(this->physicsWorld.getSun().getRenderPosition()); // move sun light
  else
    assert(this->pointLights[0] && "[Scene] ASSERT: No sun to update position");
}

// Setters
void Scene::addCamera(std::unique_ptr<Camera> camera)
{
  if (!this->activeCamera)
    this->activeCamera = camera.get();

  this->cameras.push_back(std::move(camera));
}

void Scene::addRenderable(Renderable *object)
{
  this->renderable.push_back(object);
}

void Scene::addUpdatable(Updatable *object)
{
  this->updatable.push_back(object);
}

void Scene::addTrail(std::unique_ptr<Trail> trail)
{
  this->trailViews.push_back(trail.get());
  this->trails.push_back(std::move(trail));
}

void Scene::addPointLight(std::unique_ptr<PointLight> pointLight)
{
  this->pointLightViews.push_back(pointLight.get());
  this->pointLights.push_back(std::move(pointLight));
}

void Scene::addDirLight(std::unique_ptr<DirectionalLight> directionalLight)
{
  this->directionalLight = std::move(directionalLight);
}
void Scene::addSkybox(std::unique_ptr<Skybox> skybox)
{
  this->skyboxesViews.push_back(skybox.get());
  this->skyboxes.push_back(std::move(skybox));
}

void Scene::increaseCameraSpeed(double percentage)
{
  if (this->activeCamera)
    this->activeCamera->increaseMovementSpeed(percentage);
  else
    Logger::logWarning("Scene", "No active camera to increase speed");
}

void Scene::decreaseCameraSpeed(double percentage)
{
  if (this->activeCamera)
    this->activeCamera->decreaseMovementSpeed(percentage);
  else
    Logger::logWarning("Scene", "No active camera to decrease speed");
}

// Getters
const Camera &Scene::getActiveCamera() const
{
  if (!this->activeCamera)
    throw std::runtime_error("[Scene] RUNTIME ERROR: No active camera");

  return *this->activeCamera;
};
const Skybox &Scene::getActiveSkybox() const
{
  if (!this->skybox)
    throw std::runtime_error("[Scene] RUNTIME ERROR: No active skybox");

  return *this->skybox;
};
const glm::vec3 Scene::getActiveCameraPosition() const
{
  if (!this->activeCamera)
    throw std::runtime_error("[Scene] RUNTIME ERROR: No active camera, can not get position");

  return this->activeCamera->getPosition();
};
const std::vector<Renderable *> &Scene::getRenderable() const
{
  if (this->renderable.empty())
    Logger::logWarning("Scene", "Renderable is empty");

  return this->renderable;
};
const std::vector<PointLight *> &Scene::getPointLights() const
{
  if (this->pointLightViews.empty())
    Logger::logWarning("Scene", "Point light views are empty");

  return this->pointLightViews;
};
const DirectionalLight *Scene::getDirLight() const
{
  if (!this->directionalLight)
    Logger::logWarning("Scene", "No directional light");

  return this->directionalLight.get();
};
const std::vector<Trail *> &Scene::getTrails() const
{
  if (this->trails.empty())
    Logger::logWarning("Scene", "Trails are empty");

  return this->trailViews;
};
const PhysicsWorld &Scene::getPhysicsWorld() const
{
  return this->physicsWorld;
}
