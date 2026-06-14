#pragma once

#include "camera/camera.h"

#include "scene/frameContext.h"

#include "scene/light/pointLight.h"
#include "scene/light/directionalLight.h"

#include "physics/physicsWorld.h"
#include "physics/structs/gravityField.h"
#include "physics/structs/tidalParameters.h"

#include "render/renderContext.h"

#include "graphics/model.h"
#include "graphics/skybox.h"

class Shader;
class Moon;
class Object;
class WisdomHolman;
class ResourceManager;
class ThreadPool;
class Updatable;
class Renderable;
class ModelSource;

struct KeplerElements;
struct RotationalElements;
struct HapkeParameters;
struct Radii;

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
  PhysicsWorld physicsWorld;
  Camera *activeCamera;
  Skybox *skybox;

  std::vector<Updatable *> updatable;
  std::vector<Renderable *> renderable;

  std::vector<std::unique_ptr<Camera>> cameras;
  std::vector<Camera *> cameraViews;

  std::vector<std::unique_ptr<Skybox>> skyboxes;
  std::vector<Skybox *> skyboxesViews;

  std::vector<std::unique_ptr<PointLight>> pointLights;
  std::vector<PointLight *> pointLightViews;

  std::unique_ptr<DirectionalLight> directionalLight;

  std::vector<std::unique_ptr<Trail>> trails;
  std::vector<Trail *> trailViews;

  Planet *createPlanet(std::string name, std::string material_name, double mu,
                       Radii radii, Object *centralBody, const KeplerElements keplerElements, const RotationalElements rotationalElements, double timeAfterJD2000, GravityField gravityField = GravityField(), TidalParameters tidalParameters = TidalParameters());

  Star *createStar(std::string name, std::string material_name, double mu,
                   Radii radii, double luminosity, const RotationalElements rotationalElements, double timeAfterJD2000, glm::dvec3 position = glm::dvec3(0.0), glm::dvec3 velocity = glm::dvec3(0.0));

  Moon *createMoon(std::string name, std::string material_name, double mu,
                   Radii radii, Planet *centralBody, const KeplerElements &keplerElements, const RotationalElements rotationalElements, const HapkeParameters &hapkeParameters, double timeAfterJD2000, GravityField gravityField = GravityField(), TidalParameters tidalParameters = TidalParameters());

  void addLayerToModelSource(std::string name, std::string material_name, ModelSource *object);

  AsteroidSystem *createAsteroidSystem(Object *centralBody, unsigned amount, double innerEdge, double outerEdge, double timeAfterJD2000);

public:
  Scene(ResourceManager &resourceManager, ThreadPool &threadPool);
  ~Scene();

  // Process functions
  void init(RenderContext &renderCtx, double startTime);
  void processKeyboard(CameraMovement direction, float deltaTime);
  void processMouseMovement(const float &xpos, const float &ypos);
  void processMouseScroll(float yoffset);

  void update(RenderContext &renderCtx);

  // Setters
  void addRenderable(Renderable *object);
  void addUpdatable(Updatable *object);
  void addTrail(std::unique_ptr<Trail> trail);
  void addPointLight(std::unique_ptr<PointLight> pointLight);
  void addDirLight(std::unique_ptr<DirectionalLight> directionalLight);
  void addCamera(std::unique_ptr<Camera> camera);
  void addSkybox(std::unique_ptr<Skybox> skybox);

  void increaseCameraSpeed(double percentage = 10.0);
  void decreaseCameraSpeed(double percentage = 10.0);

  // Getters
  const Camera &getActiveCamera() const;
  const Skybox &getActiveSkybox() const;

  const glm::vec3 getActiveCameraPosition() const;
  const std::vector<Renderable *> &getRenderable() const;
  const std::vector<PointLight *> &getPointLights() const;
  const DirectionalLight *getDirLight() const;
  const std::vector<Trail *> &getTrails() const;

  const PhysicsWorld &getPhysicsWorld() const;
};
