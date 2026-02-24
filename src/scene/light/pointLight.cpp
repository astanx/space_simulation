#include "scene/light/pointLight.h"

#include "graphics/shader.h"

// Constructor/Descructor
PointLight::PointLight(glm::vec3 position, glm::vec3 color, float luminosity, float radius)
{
  this->position = position;
  this->color = color;
  this->luminosity = luminosity;
  this->radius = radius;
};

// Public functions
void PointLight::sendToShader(Shader &program)
{
  program.setVec3f(this->position, "pointLight.position");
  program.set1f(this->luminosity, "pointLight.luminosity");
  program.setVec3f(this->color, "pointLight.color");
  program.set1f(this->radius, "pointLight.radius");
}

void PointLight::move(glm::vec3 position)
{
  this->position = position;
}