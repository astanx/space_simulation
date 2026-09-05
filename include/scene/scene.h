#pragma once

#include "scene/frameContext.h"
#include "scene/world/IsimulationWorld.h"
#include "scene/world/simulationWorld.h"

#include "camera/cameraMovement.h"

#include "resources/precision.h"

#include <memory>
#include <variant>
#include <glm/glm.hpp>

class ResourceManager;
class RenderQueue;
class ThreadPool;
class PointLight;
class DirectionalLight;
class Skybox;
struct RenderContext;

class Scene
{
private:
  std::variant<SimulationWorld<float>, SimulationWorld<double>> world;

public:
  Scene();
  ~Scene();

  // Process functions
  void initGPUWorld(ResourceManager &manager);
  void initCPUWorld(ThreadPool &threadPool);

  void init(RenderContext &renderCtx, ResourceManager &resourceManager, ThreadPool &threadPool, const Precision &precision, double startTime);
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

  ISimulationWorld &getSimulationWorld();
};