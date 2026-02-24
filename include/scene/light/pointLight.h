#pragma once

#include "graphics/shader.h"

#include <glm/glm.hpp>

class PointLight
{
protected:
  glm::vec3 position;
  glm::vec3 color;
  float luminosity;
  float radius;

public:
  PointLight(glm::vec3 position, glm::vec3 color, float luminosity, float radius);
  ~PointLight() = default;

  void sendToShader(Shader &program);
  void move(glm::vec3 position);

  inline const glm::vec3 &getPosition() const { return this->position; }
  inline const glm::vec3 &getColor() const { return this->color; }
  inline float getLuminosity() const { return this->luminosity; }
  inline float getRadius() const { return this->radius; }
};