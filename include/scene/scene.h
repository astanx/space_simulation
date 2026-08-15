#pragma once

#include "camera/camera.h"

#include "scene/frameContext.h"

#include "scene/light/pointLight.h"
#include "scene/light/directionalLight.h"

#include "scene/world/simulationWorld.h"

#include "render/renderContext.h"

#include "graphics/model.h"
#include "graphics/skybox.h"

class ResourceManager;
class RenderQueue;
class ThreadPool;

class Scene
{
private:
  SimulationWorld world;

  Camera *activeCamera;
  Skybox *skybox;

  std::vector<std::unique_ptr<Camera>> cameras;
  std::vector<Camera *> cameraViews;

  std::vector<std::unique_ptr<Skybox>> skyboxes;
  std::vector<Skybox *> skyboxesViews;

  std::unique_ptr<PointLight> pointLight;
  std::unique_ptr<DirectionalLight> directionalLight;

public:
  Scene() = default;
  ~Scene() = default;

  // Process functions
  void init(RenderContext &renderCtx, ResourceManager &resourceManager, ThreadPool &threadPool, double startTime);
  void processKeyboard(CameraMovement direction, float deltaTime);
  void processMouseMovement(const float &xpos, const float &ypos);
  void processMouseScroll(float yoffset);

  void update(RenderQueue &queue, RenderContext &renderCtx);

  // Setters
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
  const PointLight *getPointLight() const;
  const DirectionalLight *getDirLight() const;

  SimulationWorld &getSimulationWorld();
};
