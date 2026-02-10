#pragma once

#include "graphics/model.h"
#include "graphics/skybox.h"
#include "physics/orbitalObject.h"
#include "camera/camera.h"
#include "scene/light/pointLight.h"
#include "scene/light/directionalLight.h"
#include "physics/asteroidSystem.h"
#include "physics/star.h"

class Shader;
class Planet;
class Moon;
class Object;
class ResourceManager;
class ThreadPool;

struct KeplerElements;

struct CameraGPU
{
  glm::mat4 ProjectionMatrix;
  glm::mat4 ViewMatrix;
  glm::vec4 camPosition;
};

class Scene
{
private:
  ResourceManager &resourceManager;
  ThreadPool &threadPool;
  Camera *activeCamera;
  Skybox *skybox;

  std::vector<std::unique_ptr<Model>> models;

  std::vector<std::unique_ptr<OrbitalObject>> orbitalObjects;
  std::vector<OrbitalObject *> orbitalObjectViews;

  std::vector<std::unique_ptr<Star>> stars;
  Star *sun;

  std::vector<std::unique_ptr<AsteroidSystem>> asteroidSystems;
  std::vector<AsteroidSystem *> asteroidSystemViews;

  std::vector<Object *> objects;

  std::vector<std::unique_ptr<Trail>> trails;
  std::vector<Trail *> trailViews;

  std::vector<std::unique_ptr<Camera>> cameras;
  std::vector<Camera *> cameraViews;

  std::vector<std::unique_ptr<Skybox>> skyboxes;
  std::vector<Skybox *> skyboxesViews;

  std::vector<std::unique_ptr<PointLight>> pointLights;
  std::vector<PointLight *> pointLightViews;

  std::unique_ptr<DirectionalLight> directionalLight;

  void halfKick(double dt);
  void drift(double dt);
  void wisdomHolman(double dt);

public:
  Scene(ResourceManager &resourceManager, ThreadPool &threadPool);
  ~Scene() = default;

  Planet *createPlanet(std::string name, std::string material_name, double mu,
                       double radius, Object *centralBody, const KeplerElements keplerElements);

  Star *createStar(std::string name, std::string material_name, double mu,
                   double radius, glm::dvec3 position = glm::dvec3(0.0), glm::dvec3 velocity = glm::dvec3(0.0));

  Moon *createMoon(std::string name, std::string material_name, double mu,
                   double radius, Planet *centralBody, const KeplerElements keplerElements);

  AsteroidSystem *createAsteroidSystem(Object *centralBody, unsigned amount, double innerEdge, double outerEdge);

  // Process functions
  void init();
  void processKeyboard(CameraMovement direction, float deltaTime);
  void processMouseMovement(const float &xpos, const float &ypos);
  void processMouseScroll(float yoffset);

  void update(double dt);

  // Setters
  void addModel(std::unique_ptr<Model> model);
  void addObject(Object *object);
  void addOrbitalObject(std::unique_ptr<OrbitalObject> orbitalObject);
  void addAsteroidSystem(std::unique_ptr<AsteroidSystem> asteroidSystem);
  void addStar(std::unique_ptr<Star> star);
  void addTrail(std::unique_ptr<Trail> trail);
  void addPointLight(std::unique_ptr<PointLight> pointLight);
  void addDirLight(std::unique_ptr<DirectionalLight> directionalLight);
  void addCamera(std::unique_ptr<Camera> camera);
  void addSkybox(std::unique_ptr<Skybox> skybox);

  // Getters
  const Camera &getActiveCamera() const;
  const Skybox &getActiveSkybox() const;
  const Star &getSun() const;

  const glm::vec3 getActiveCameraPosition() const;
  const std::vector<Object *> &getObjects() const;
  const std::vector<PointLight *> &getPointLights() const;
  const DirectionalLight *getDirLight() const;
  const std::vector<Trail *> &getTrails() const;
  const std::vector<AsteroidSystem *> &getAsteroidSystems() const;
};
