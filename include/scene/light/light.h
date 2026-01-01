#pragma once

#include <glm/glm.hpp>

class Shader;

class Light
{
protected:
  float intensity;
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;

public:
  Light(glm::vec3 ambient,
        glm::vec3 diffuse,
        glm::vec3 specular, float intensity = 1.f);

  ~Light();

  virtual void sendToShader(Shader &program) = 0;
};
