#include "scene/scene.h"

#include "debug/logger.h"

#include "graphics/shader.h"
#include "graphics/mesh.h"
#include "physics/planet.h"
#include "physics/orbit.h"
#include "physics/star.h"
#include "physics/moon.h"
#include "physics/constants.h"

#include "resources/resources.h"
#include "resources/resourceManager.h"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

// Private functions
void Scene::halfKick(double dt)
{
  for (WisdomHolman *&object : this->wisdomHolmanObjects)
    object->halfKick(this->objects, dt);
}

void Scene::drift(double dt)
{
  for (WisdomHolman *&object : this->wisdomHolmanObjects)
    object->drift(dt);
}

void Scene::wisdomHolman(double dt)
{
  this->halfKick(dt);

  this->drift(dt);

  this->halfKick(dt);
}

Planet *Scene::createPlanet(std::string name, std::string material_name, double mu,
                            double radius, Object *centralBody, const KeplerElements keplerElements)
{
  Mesh &mesh = this->resourceManager.GetMesh(name);
  Material &mat = this->resourceManager.GetMaterial(material_name);
  std::unique_ptr<Model> model = std::make_unique<Model>(glm::dvec3(0.0), mat, mesh);

  std::unique_ptr<Planet> planet = std::make_unique<Planet>(centralBody, mu, radius, keplerElements);

  planet->addMainLayer(std::move(model));

  Planet *ptr = planet.get();

  if (planet->getUseTrail())
    this->addTrail(planet->generateTrail());

  this->addPlanetarObject(std::move(planet));
  this->addRenderable(ptr);
  this->addUpdatable(ptr);
  this->addObject(ptr);
  this->addWisdomHolman(ptr);

  return ptr;
}

Star *Scene::createStar(std::string name, std::string material_name, double mu,
                        double radius, double luminosity, glm::dvec3 position, glm::dvec3 velocity)
{
  Mesh &mesh = this->resourceManager.GetMesh(name);
  Material &mat = this->resourceManager.GetMaterial(material_name);
  std::unique_ptr<Model> model = std::make_unique<Model>(glm::dvec3(0.0), mat, mesh);

  std::unique_ptr<Star> star = std::make_unique<Star>(mu, radius, luminosity, position, velocity);

  star->addMainLayer(std::move(model));

  Star *ptr = star.get();
  this->addStar(std::move(star));
  this->addRenderable(ptr);
  this->addUpdatable(ptr);
  this->addObject(ptr);
  this->addWisdomHolman(ptr);

  return ptr;
}

Moon *Scene::createMoon(std::string name, std::string material_name, double mu,
                        double radius, Planet *centralBody, const KeplerElements &keplerElements, const HapkeParameters &hapkeParameters)
{
  Mesh &mesh = this->resourceManager.GetMesh(name);
  Material &mat = this->resourceManager.GetMaterial(material_name);
  std::unique_ptr<Model> model = std::make_unique<Model>(glm::dvec3(0.0), mat, mesh);

  std::unique_ptr<Moon> moon = std::make_unique<Moon>(centralBody, mu, radius, keplerElements, hapkeParameters);

  moon->addMainLayer(std::move(model));
  if (moon->getUseTrail())
    this->addTrail(moon->generateTrail());

  Moon *ptr = moon.get();

  assert(centralBody && "[Scene] ASSERT: No central body for moon");

  centralBody->addMoon(std::move(moon));
  this->addUpdatable(ptr);
  this->addRenderable(ptr);
  this->addObject(ptr);
  this->addWisdomHolman(ptr);

  return ptr;
}

AsteroidSystem *Scene::createAsteroidSystem(Object *centralBody, unsigned amount, double innerEdge, double outerEdge)
{
  std::unique_ptr<AsteroidSystem> system = std::make_unique<AsteroidSystem>(centralBody, amount,
                                                                            innerEdge, outerEdge,
                                                                            &this->resourceManager.GetMaterial(Res::ASTEROID_MATERIAL), this->threadPool);
  AsteroidSystem *ptr = system.get();
  this->addAsteroidSystem(std::move(system));
  this->addUpdatable(ptr);
  this->addWisdomHolman(ptr);

  return ptr;
}

void Scene::addLayerToModelSource(std::string name, std::string material_name, ModelSource *object)
{
  Mesh &mesh = this->resourceManager.GetMesh(name);
  Material &mat = this->resourceManager.GetMaterial(material_name);
  std::unique_ptr<Model> model = std::make_unique<Model>(glm::dvec3(0.0), mat, mesh);

  object->addLayer(std::move(model));
}

// Constructor/Destructor
Scene::Scene(ResourceManager &resourceManager, ThreadPool &threadPool) : threadPool(threadPool), resourceManager(resourceManager)
{
  this->activeCamera = nullptr;
  this->skybox = nullptr;
}

// Process functions
void Scene::init(RenderContext &renderCtx)
{
  Star *sunPtr = createStar(Res::SUN, Res::SUN_MATERIAL, sunMu, sunRadii.mean, sunLuminosity, sunPos);
  this->sun = sunPtr;
  createPlanet(Res::MERCURY, Res::MERCURY_MATERIAL, mercuryMu, mercuryRadii.mean, sunPtr, mercuryElements);
  Planet *venusPtr = createPlanet(Res::VENUS, Res::VENUS_MATERIAL, venusMu, venusRadii.mean, sunPtr, venusElements);
  addLayerToModelSource(Res::VENUS_ATMOSPHERE, Res::VENUS_ATMOSPHERE_MATERIAL, venusPtr);
  Planet *earthPtr = createPlanet(Res::EARTH, Res::EARTH_MATERIAL, earthMu, earthRadii.mean, sunPtr, earthElements);
  addLayerToModelSource(Res::EARTH_ATMOSPHERE, Res::EARTH_ATMOSPHERE_MATERIAL, earthPtr);
  createMoon(Res::MOON, Res::MOON_MATERIAL, moonMu, moonRadii.mean, earthPtr, moonElements, moonHapkeParameters);
  createPlanet(Res::MARS, Res::MARS_MATERIAL, marsMu, marsRadii.mean, sunPtr, marsElements);
  createAsteroidSystem(sunPtr, 20000, INNER_ASTEROID_BELT_EDGE, OUTER_ASTEROID_BELT_EDGE);
  createPlanet(Res::JUPITER, Res::JUPITER_MATERIAL, jupiterMu, jupiterRadii.mean, sunPtr, jupiterElements);

  // std::unique_ptr<PointLight> pointLight = std::make_unique<PointLight>(
  //     sunPos,
  //     glm::vec3(0.05f),
  //     glm::vec3(1.0f),
  //     glm::vec3(1.0f),
  //     25.f,
  //     1.f,
  //     0.00009f,
  //     0.0000032f);

  std::unique_ptr<PointLight> pointLight = std::make_unique<PointLight>(
      this->sun->getRenderPosition(),
      glm::vec3(1.0f),
      this->sun->getLuminosity(),
      this->sun->getRadius());
  this->addPointLight(std::move(pointLight));

  // std::unique_ptr<DirectionalLight> dirLight = std::make_unique<DirectionalLight>(
  //     glm::vec3(1.2f, 1.0f, 2.0f),
  //     glm::vec3(0.5f),
  //     glm::vec3(1.0f),
  //     glm::vec3(1.0f), 1.f);
  // this->addDirLight(std::move(dirLight));

  std::unique_ptr<Camera> cam = std::make_unique<Camera>(sunPos,
                                                         glm::vec3(0.0f, 0.0f, -1.0f),
                                                         glm::vec3(0.0f, 1.0f, 0.0f),
                                                         renderCtx.frameCtx.width, renderCtx.frameCtx.height);
  this->addCamera(std::move(cam));
  activeCamera = this->cameras.back().get();

  // std::vector<std::string> faces =
  //     {
  //         "assets/skybox/right.png",
  //         "assets/skybox/left.png",
  //         "assets/skybox/top.png",
  //         "assets/skybox/bottom.png",
  //         "assets/skybox/front.png",
  //         "assets/skybox/back.png"};

  // std::unique_ptr<Skybox> sb = std::make_unique<Skybox>(faces);
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
    this->wisdomHolman(renderCtx.deltaTime);

  // for (size_t i = 0; i < objects.size(); ++i)
  // {
  //   for (size_t j = i + 1; j < objects.size(); ++j)
  //   {
  //     objects[i]->applyGravitation(*objects[j]);
  //     objects[j]->applyGravitation(*objects[i]);
  //   }
  // }

  renderCtx.frameCtx.camPosition = this->activeCamera->getPosition();

  Frustum frustum = this->activeCamera->getFrustum(renderCtx.frameCtx.aspect);

  for (Updatable *&object : this->updatable)
    object->update(renderCtx.deltaTime, renderCtx.frameCtx, &frustum);

  for (std::unique_ptr<Trail> &trail : this->trails)
    trail->update(renderCtx.frameCtx.camPosition);

  if (this->pointLights[0])
    this->pointLights[0]->move(this->sun->getRenderPosition()); // move sun light
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

void Scene::addObject(Object *object)
{
  this->objects.push_back(object);
}

void Scene::addRenderable(Renderable *object)
{
  this->renderable.push_back(object);
}

void Scene::addUpdatable(Updatable *object)
{
  this->updatable.push_back(object);
}
void Scene::addWisdomHolman(WisdomHolman *object)
{
  this->wisdomHolmanObjects.push_back(object);
}
void Scene::addPlanetarObject(std::unique_ptr<Planet> planetarObject)
{
  this->planetarObjectViews.push_back(planetarObject.get());
  this->planetarObjects.push_back(std::move(planetarObject));
}

void Scene::addStar(std::unique_ptr<Star> star)
{
  this->stars.push_back(std::move(star));
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

void Scene::addAsteroidSystem(std::unique_ptr<AsteroidSystem> asteroidSystem)
{
  this->asteroidSystemViews.push_back(asteroidSystem.get());
  this->asteroidSystems.push_back(std::move(asteroidSystem));
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
const Star &Scene::getSun() const
{
  if (!this->sun)
    throw std::runtime_error("[Scene] RUNTIME ERROR: No sun");

  return *this->sun;
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
const std::vector<AsteroidSystem *> &Scene::getAsteroidSystems() const
{
  if (this->asteroidSystemViews.empty())
    Logger::logWarning("Scene", "Asteroid systems are empty");

  return this->asteroidSystemViews;
};
const std::vector<Planet *> &Scene::getPlanetarObjects() const
{
  if (this->planetarObjectViews.empty())
    Logger::logWarning("Scene", "Planets are empty");

  return this->planetarObjectViews;
};