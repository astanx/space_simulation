#pragma once

#include "graphics/model.h"
#include "graphics/mesh.h"
#include "graphics/skybox.h"
#include "physics/object.h"
#include "camera/camera.h"
#include "scene/light/pointLight.h"
#include "scene/light/directionalLight.h"
#include "resources/resourceManager.h"
#include "physics/keplerElements.h"

class Shader;
class Planet;
class Star;
class Moon;

class Scene
{
private:
  ResourceManager *resourceManager;
  Camera *activeCamera;
  Skybox* skybox;

  std::vector<std::unique_ptr<Model>> models;
  std::vector<std::unique_ptr<Object>> objects;
  std::vector<std::unique_ptr<Camera>> cameras;
  std::vector<std::unique_ptr<Skybox>> skyboxes;

  std::vector<std::unique_ptr<PointLight>> pointLights;
  std::unique_ptr<DirectionalLight> directionalLight;

  std::vector<std::unique_ptr<Model>> asteroids;
  Material* asteroid_material;

public:
  Scene(ResourceManager *resourceManager);
  ~Scene() = default;

  Planet *createPlanet(std::string name, std::string material_name, double mu,
                       double radius, Object *centralBody, const KeplerElements keplerElements);

  Star *createStar(std::string name, std::string material_name, double mu,
                          double radius, glm::dvec3 position = glm::dvec3(0.0), glm::dvec3 velocity = glm::dvec3(0.0));

  Moon *createMoon(std::string name, std::string material_name, double mu,
                   double radius, Planet *centralBody, const KeplerElements keplerElements);

  void createAsteroids(unsigned amount, double innerEdge, double outerEdge);

  // Process functions
  void init(float width, float height);
  void processKeyboard(CameraMovement direction, float deltaTime);
  void processMouseMovement(const float &xpos, const float &ypos);
  void processMouseScroll(float yoffset);

  void update(float dt);
  void render(Shader *shader, int framebufferWidth, int framebufferHeight, float dt, Shader *skyboxShader, Shader* instanceShader);
  void renderSkybox(Shader *skyboxShader, float aspectRatio);

  void sendLightsToShader(Shader &shader);
  void sendCameraToShader(Shader &shader, float aspectRatio);

  // Setters
  void addModel(std::unique_ptr<Model> model);
  void addObject(std::unique_ptr<Object> object);
  void addPointLight(std::unique_ptr<PointLight> pointLight);
  void addDirLight(std::unique_ptr<DirectionalLight> directionalLight);
  void addCamera(std::unique_ptr<Camera> camera);
  void addSkybox(std::unique_ptr<Skybox> skybox);
  // Getters
  Camera &getActiveCamera();
  const glm::vec3 getActiveCameraPosition() const;
  const std::vector<std::unique_ptr<PointLight>> &getPointLights() const;
  const std::unique_ptr<DirectionalLight> &getDirLight() const;
};
