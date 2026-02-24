#include "scene/light/phongPointLight.h"

#include "graphics/shader.h"

// Constructor/Descructor
PhongPointLight::PhongPointLight(glm::vec3 position, glm::vec3 ambient,
                                 glm::vec3 diffuse,
                                 glm::vec3 specular, float intensity, float constant, float linear, float quadratic) : Light(ambient, diffuse, specular, intensity)
{
  this->position = position;
  this->constant = constant;
  this->linear = linear;
  this->quadratic = quadratic;
};

// Public functions
void PhongPointLight::sendToShader(Shader &program)
{
  program.setVec3f(this->position, "phongPointLight.position");
  program.set1f(this->intensity, "phongPointLight.intensity");

  program.setVec3f(this->ambient, "phongPointLight.ambient");
  program.setVec3f(this->diffuse, "phongPointLight.diffuse");
  program.setVec3f(this->specular, "phongPointLight.specular");

  program.set1f(this->constant, "phongPointLight.constant");
  program.set1f(this->linear, "phongPointLight.linear");
  program.set1f(this->quadratic, "phongPointLight.quadratic");
}

void PhongPointLight::move(glm::vec3 position)
{
  this->position = position;
}