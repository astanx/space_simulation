#pragma once

#include <glm/glm.hpp>

struct PhongMaterialProperties
{
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
  float shininess;
  glm::vec3 emissive;

  PhongMaterialProperties(
      glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess, glm::vec3 emissive = glm::vec3(0.f))
      : ambient(ambient), diffuse(diffuse), specular(specular), shininess(shininess), emissive(emissive) {};
};