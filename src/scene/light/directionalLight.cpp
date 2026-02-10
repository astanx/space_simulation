#include "scene/light/directionalLight.h"

#include "graphics/shader.h"

// Constructor/Descructor
DirectionalLight::DirectionalLight(glm::vec3 direction, glm::vec3 ambient,
                                   glm::vec3 diffuse,
                                   glm::vec3 specular, float intensity) : Light(ambient, diffuse, specular, intensity)
{
  this->direction = direction;
};

// Public functions
void DirectionalLight::sendToShader(Shader &program)
{
  program.setVec3f(this->direction, "dirLight.direction");
  program.set1f(this->intensity, "dirLight.intensity");

  program.setVec3f(this->ambient, "dirLight.ambient");
  program.setVec3f(this->diffuse, "dirLight.diffuse");
  program.setVec3f(this->specular, "dirLight.specular");
}

void DirectionalLight::changeDirection(glm::vec3 direction)
{
  this->direction = direction;
}