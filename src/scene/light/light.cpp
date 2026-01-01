#include "scene/light/light.h"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

// Constructor and Destructor
Light::Light(glm::vec3 ambient,
             glm::vec3 diffuse,
             glm::vec3 specular, float intensity)
{
  this->ambient = ambient;
  this->diffuse = diffuse;
  this->specular = specular;
  this->intensity = intensity;
}

Light::~Light() {}