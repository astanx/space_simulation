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
  for (Object *&object : this->objects)
  {
    object->halfKick(this->objects, dt);
  }

  for (std::unique_ptr<AsteroidSystem> &asteroidSystem : this->asteroidSystems)
  {
    asteroidSystem->halfKick(this->objects, dt);
  }
}

void Scene::drift(double dt)
{
  for (Object *&object : this->objects)
  {
    object->drift(dt);
  }

  for (std::unique_ptr<AsteroidSystem> &asteroidSystem : this->asteroidSystems)
  {
    asteroidSystem->drift(dt);
  }
}

void Scene::wisdomHolman(double dt)
{
  this->halfKick(dt);

  this->drift(dt);

  this->halfKick(dt);
}

// Constructor/Destructor
Scene::Scene(ResourceManager &resourceManager, ThreadPool &threadPool) : threadPool(threadPool), resourceManager(resourceManager)
{
  this->activeCamera = nullptr;
  this->skybox = nullptr;
}

// Public functions
Planet *Scene::createPlanet(std::string name, std::string material_name, double mu,
                            double radius, Object *centralBody, const KeplerElements keplerElements)
{
  Mesh &mesh = this->resourceManager.GetMesh(name);
  Material &mat = this->resourceManager.GetMaterial(material_name);
  std::unique_ptr<Model> model = std::make_unique<Model>(glm::dvec3(0.0), mat, mesh);

  std::unique_ptr<Planet> planet = std::make_unique<Planet>(centralBody, mu, radius, keplerElements);

  planet->addModel(std::move(model));

  Planet *ptr = planet.get();

  if (planet->getUseTrail())
    this->addTrail(planet->generateTrail());

  this->addOrbitalObject(std::move(planet));
  this->addObject(ptr);

  return ptr;
}

Star *Scene::createStar(std::string name, std::string material_name, double mu,
                        double radius, glm::dvec3 position, glm::dvec3 velocity)
{
  Mesh &mesh = this->resourceManager.GetMesh(name);
  Material &mat = this->resourceManager.GetMaterial(material_name);
  std::unique_ptr<Model> model = std::make_unique<Model>(position, mat, mesh);

  std::unique_ptr<Star> star = std::make_unique<Star>(mu, radius, position, velocity);

  star->addModel(std::move(model));

  Star *ptr = star.get();
  this->addStar(std::move(star));
  this->addObject(ptr);

  return ptr;
}

Moon *Scene::createMoon(std::string name, std::string material_name, double mu,
                        double radius, Planet *centralBody, const KeplerElements keplerElements)
{
  Mesh &mesh = this->resourceManager.GetMesh(name);
  Material &mat = this->resourceManager.GetMaterial(material_name);
  std::unique_ptr<Model> model = std::make_unique<Model>(glm::dvec3(0.0), mat, mesh);

  std::unique_ptr<Moon> moon = std::make_unique<Moon>(centralBody, mu, radius, keplerElements);

  moon->addModel(std::move(model));
  if (moon->getUseTrail())
    this->addTrail(moon->generateTrail());

  Moon *ptr = moon.get();

  assert(centralBody && "[Scene] ASSERT: No central body for moon");

  centralBody->addMoon(std::move(moon));

  return ptr;
}

AsteroidSystem *Scene::createAsteroidSystem(Object *centralBody, unsigned amount, double innerEdge, double outerEdge)
{
  std::unique_ptr<AsteroidSystem> system = std::make_unique<AsteroidSystem>(centralBody, amount,
                                                                            innerEdge, outerEdge,
                                                                            &this->resourceManager.GetMaterial(Res::ASTEROID_MATERIAL), this->threadPool);
  AsteroidSystem *ptr = system.get();
  this->addAsteroidSystem(std::move(system));
  return ptr;
}

// Process functions
void Scene::init()
{
  Star *sunPtr = createStar(Res::SUN, Res::SUN_MATERIAL, sunMu, sunRadii.mean, sunPos);
  this->sun = sunPtr;
  createPlanet(Res::MERCURY, Res::MERCURY_MATERIAL, mercuryMu, mercuryRadii.mean, sunPtr, mercuryElements);
  createPlanet(Res::VENUS, Res::VENUS_MATERIAL, venusMu, venusRadii.mean, sunPtr, venusElements);
  Planet *earthPtr = createPlanet(Res::EARTH, Res::EARTH_MATERIAL, earthMu, earthRadii.mean, sunPtr, earthElements);
  createMoon(Res::MOON, Res::MOON_MATERIAL, moonMu, moonRadii.mean, earthPtr, moonElements);
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
      sunPos,
      glm::vec3(0.08f),
      glm::vec3(1.00f, 0.96f, 0.90f),
      glm::vec3(1.00f, 0.92f, 0.80f),
      25.0f,
      1.0f,
      0.00002f,
      0.0000008f);
  this->addPointLight(std::move(pointLight));

  // std::unique_ptr<DirectionalLight> dirLight = std::make_unique<DirectionalLight>(
  //     glm::vec3(1.2f, 1.0f, 2.0f),
  //     glm::vec3(0.5f),
  //     glm::vec3(1.0f),
  //     glm::vec3(1.0f), 1.f);
  // this->addDirLight(std::move(dirLight));

  std::unique_ptr<Camera> cam = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f),
                                                         glm::vec3(0.0f, 0.0f, -1.0f),
                                                         glm::vec3(0.0f, 1.0f, 0.0f));

  this->addCamera(std::move(cam));
  activeCamera = this->cameras.back().get();

  std::vector<const char *> faces =
      {

          "assets/skybox/right.png",
          "assets/skybox/left.png",
          "assets/skybox/top.png",
          "assets/skybox/bottom.png",
          "assets/skybox/front.png",
          "assets/skybox/back.png"};

  std::unique_ptr<Skybox> sb = std::make_unique<Skybox>(faces);
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

void Scene::update(double dt)
{
  this->wisdomHolman(dt);

  // for (size_t i = 0; i < objects.size(); ++i)
  // {
  //   for (size_t j = i + 1; j < objects.size(); ++j)
  //   {
  //     objects[i]->applyGravitation(*objects[j]);
  //     objects[j]->applyGravitation(*objects[i]);
  //   }
  // }

  for (Object *&object : this->objects)
  {
    object->update(dt);
  }

  for (std::unique_ptr<AsteroidSystem> &asteroidSystem : this->asteroidSystems)
  {
    asteroidSystem->update(dt);
  }

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

void Scene::addModel(std::unique_ptr<Model> model)
{
  this->models.push_back(std::move(model));
}

void Scene::addObject(Object *object)
{
  this->objects.push_back(object);
}

void Scene::addOrbitalObject(std::unique_ptr<OrbitalObject> orbitalObject)
{
  this->orbitalObjectViews.push_back(orbitalObject.get());
  this->orbitalObjects.push_back(std::move(orbitalObject));
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
const std::vector<Object *> &Scene::getObjects() const
{
  if (this->objects.empty())
    Logger::logWarning("Scene", "Objects are empty");

  return this->objects;
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