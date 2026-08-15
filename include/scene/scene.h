#pragma once

#include "scene/frameContext.h"
#include "scene/world/simulationWorld.h"

#include "camera/cameraMovement.h"

class ResourceManager;
class RenderQueue;
class ThreadPool;
struct RenderContext;

class Scene
{
private:
  SimulationWorld world;

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
  void increaseCameraSpeed(double percentage = 10.0);
  void decreaseCameraSpeed(double percentage = 10.0);

  // Getters
  const Camera &getActiveCamera();
  const Skybox &getActiveSkybox();

  const glm::vec3 getActiveCameraPosition();
  const PointLight *getPointLight();
  const DirectionalLight *getDirLight();

  SimulationWorld &getSimulationWorld();
};
