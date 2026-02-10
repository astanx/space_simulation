#include "scene/light/pointLight.h"

#include "graphics/shader.h"

// Constructor/Descructor
PointLight::PointLight(glm::vec3 position, glm::vec3 ambient,
                       glm::vec3 diffuse,
                       glm::vec3 specular, float intensity, float constant, float linear, float quadratic) : Light(ambient, diffuse, specular, intensity)
{
  this->position = position;
  this->constant = constant;
  this->linear = linear;
  this->quadratic = quadratic;
};

// Public functions
void PointLight::sendToShader(Shader &program)
{
  program.setVec3f(this->position, "pointLight.position");
  program.set1f(this->intensity, "pointLight.intensity");

  program.setVec3f(this->ambient, "pointLight.ambient");
  program.setVec3f(this->diffuse, "pointLight.diffuse");
  program.setVec3f(this->specular, "pointLight.specular");

  program.set1f(this->constant, "pointLight.constant");
  program.set1f(this->linear, "pointLight.linear");
  program.set1f(this->quadratic, "pointLight.quadratic");
}

void PointLight::move(glm::vec3 position)
{
  this->position = position;
}