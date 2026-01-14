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
};

enum UBOBindingPoints {
    CAMERA_BINDING = 0,
    DIR_LIGHT_BINDING = 1,
    POINT_LIGHT_BINDING = 2
};

class DirectionalLight;
class PointLight;

class LightManager
{
private:
  GLuint dirUBO = 0;
  GLuint pointUBO;
  GLuint pointSSBO = 0;

public:
  LightManager() = default;
  ~LightManager() = default;

  void updateDirUBO(const DirectionalLight &dirLight);
  void updatePointUBO(const PointLight &pointLight);
  void updateSSBO(std::vector<PointLight> &pointLights);

  void bindDirLight(GLuint &programID);
  void bindPointLightUBO(GLuint &programID);
  void bindPointLightSSBO(GLuint &programID);

  GLuint &getDirUBO() { return this->dirUBO; };
  GLuint &getPointSSBO() { return this->pointSSBO; };
  GLuint &getPointUBO() { return this->pointUBO; };
};