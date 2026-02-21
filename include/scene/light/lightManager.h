#pragma once

#include "graphics/buffers/buffer.h"

#include <GL/glew.h>
#include "glm/glm.hpp"

#include <memory>

struct DirLightGPU
{
  glm::vec4 direction;
  glm::vec4 ambient;
  glm::vec4 diffuse;
  glm::vec4 specular;
  float intensity;
  int enabled;
  glm::vec2 _pad0;
};

struct PointLightGPU
{
  glm::vec4 position;

  glm::vec4 ambient;
  glm::vec4 diffuse;
  glm::vec4 specular;

  float intensity;
  float constant;
  float linear;
  float quadratic;
  int enabled;
  glm::vec3 _pad0;
};

class DirectionalLight;
class PointLight;
class Scene;

class LightManager
{
private:
  std::unique_ptr<Buffer> dirUBO;
  std::unique_ptr<Buffer> pointUBO;
  // GLuint pointSSBO = 0;

  void initDirUBO();
  void initPointUBO();

public:
  LightManager(Scene &scene);
  ~LightManager() = default;

  void updateDirUBO(const DirectionalLight *dirLight, int enabled = 1);
  void updatePointUBO(const PointLight *pointLight, int enabled = 1);

  void maskDirUBO();
  void maskPointUBO();

  // void updateSSBO(std::vector<PointLight> &pointLights);

  void initDirLightUBOBinding(GLuint &programID);
  void initPointLightUBOBinding(GLuint &programID);

  void bindDirLightUBO();
  void bindPointLightUBO();
  // void bindPointLightSSBO(GLuint &programID);
};