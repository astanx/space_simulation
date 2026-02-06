#pragma once

#include "graphics/model.h"
#include "graphics/mesh.h"
#include "graphics/skybox.h"
#include "physics/object.h"
#include "physics/orbitalObject.h"
#include "camera/camera.h"
#include "scene/light/pointLight.h"
#include "scene/light/directionalLight.h"
#include "scene/light/lightManager.h"
#include "scene/shadow/directionalShadow.h"
#include "scene/shadow/pointShadow.h"
#include "scene/shadow/shadowManager.h"
#include "resources/resourceManager.h"
#include "physics/structs/keplerElements.h"
#include "physics/asteroidSystem.h"
#include "resources/threadPool.h"

class Shader;
class Planet;
class Star;
class Moon;

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

  std::vector<std::unique_ptr<Object>> objects;
  std::vector<Star *> stars;
  Star *sun;

  std::vector<std::unique_ptr<Trail>> trails;
  std::vector<std::unique_ptr<Camera>> cameras;
  std::vector<std::unique_ptr<Skybox>> skyboxes;

  std::vector<std::unique_ptr<PointLight>> pointLights;
  std::unique_ptr<DirectionalLight> directionalLight;

  std::vector<std::unique_ptr<AsteroidSystem>> asteroidSystems;

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
  void init(float width, float height);
  void processKeyboard(CameraMovement direction, float deltaTime);
  void processMouseMovement(const float &xpos, const float &ypos);
  void processMouseScroll(float yoffset);

  void update(float dt);

  // Setters
  void addModel(std::unique_ptr<Model> model);
  void addObject(std::unique_ptr<Object> object);
  void addTrail(std::unique_ptr<Trail> trail);
  void addPointLight(std::unique_ptr<PointLight> pointLight);
  void addDirLight(std::unique_ptr<DirectionalLight> directionalLight);
  void addCamera(std::unique_ptr<Camera> camera);
  void addSkybox(std::unique_ptr<Skybox> skybox);

  // Getters
  Camera &getActiveCamera() { return *this->activeCamera; };
  Skybox &getActiveSkybox() { return *this->skybox; };
  Star &getSun() { return *this->sun; };

  const glm::vec3 getActiveCameraPosition() const { return this->activeCamera->getPosition(); };
  const std::vector<std::unique_ptr<PointLight>> &getPointLights() const { return this->pointLights; };
  const std::unique_ptr<DirectionalLight> &getDirLight() const { return this->directionalLight; };
  const std::vector<std::unique_ptr<Object>> &getObjects() const { return this->objects; };
  const std::vector<std::unique_ptr<Trail>> &getTrails() const { return this->trails; };
  const std::vector<std::unique_ptr<AsteroidSystem>> &getAsteroidSystems() const { return this->asteroidSystems; };
};
