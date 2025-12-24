#pragma once

#include <glm/glm.hpp>

class Shader;

class Light
{
private:
  glm::vec3 position;
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;

  float constant;
  float linear;
  float quadratic;

public:
  Light(
      glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic);

  ~Light();

  void sendToShader(Shader &program);

  void move(glm::vec3 position);
};
