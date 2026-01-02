#pragma once

#include "graphics/model.h"
#include "camera/camera.h"
#include "scene/light/pointLight.h"
#include "scene/light/directionalLight.h"
#include "resources/resourceManager.h"

class Shader;

class Scene
{
private:
  ResourceManager *resourceManager;
  Camera *activeCamera;

  std::vector<std::unique_ptr<Model>> models;
  std::vector<std::unique_ptr<Camera>> cameras;

  std::vector<std::unique_ptr<PointLight>> pointLights;
  std::unique_ptr<DirectionalLight> directionalLight;

  // Skybox* skybox;

  // PhysicsWorld* physics;
public:
  Scene(ResourceManager *resourceManager);
  ~Scene() = default;

  // Process functions
  void init(float width, float height);
  void processKeyboard(CameraMovement direction, float deltaTime);
  void processMouseMovement(const float &xpos, const float &ypos);
  void processMouseScroll(float yoffset);

  void update(float dt);
  void render(Shader *shader, int framebufferWidth, int framebufferHeight);

  void sendLightsToShader(Shader &shader);
  void sendCameraToShader(Shader &shader, float aspectRatio);

  // Setters
  void addModel(std::unique_ptr<Model> model);
  void addPointLight(std::unique_ptr<PointLight> pointLight);
  void addDirLight(std::unique_ptr<DirectionalLight> directionalLight);
  void addCamera(std::unique_ptr<Camera> camera);

  // Getters
  Camera &getActiveCamera();
  const glm::vec3 getActiveCameraPosition() const;
  const std::vector<std::unique_ptr<PointLight>> &getPointLights() const;
  const std::unique_ptr<DirectionalLight> &getDirLight() const;
};
