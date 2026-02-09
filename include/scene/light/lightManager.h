#pragma once

#include <GL/glew.h>
#include "glm/glm.hpp"
#include <vector>

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

enum LightUBOBindingPoints
{
  CAMERA_BINDING = 0,
  DIR_LIGHT_BINDING = 1,
  POINT_LIGHT_BINDING = 2
};

class DirectionalLight;
class PointLight;
class Scene;

class LightManager
{
private:
  GLuint dirUBO = 0;
  GLuint pointUBO = 0;
  // GLuint pointSSBO = 0;

  void initDirUBO();
  void initPointUBO();

public:
  LightManager(Scene& scene);
  ~LightManager() = default;

  void updateDirUBO(const DirectionalLight *dirLight, int enabled = 1);
  void updatePointUBO(const PointLight *pointLight, int enabled = 1);

  void maskDirUBO();
  void maskPointUBO();

  // void updateSSBO(std::vector<PointLight> &pointLights);

  void bindDirLight(GLuint &programID);
  void bindPointLightUBO(GLuint &programID);
  // void bindPointLightSSBO(GLuint &programID);
};