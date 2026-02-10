#pragma once

#include <glm/glm.hpp>

struct MaterialProperties
{
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
  float shininess;

  MaterialProperties(
      glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess)
      : ambient(ambient), diffuse(diffuse), specular(specular), shininess(shininess) {};
};