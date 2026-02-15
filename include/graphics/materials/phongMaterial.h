#pragma once

#include "graphics/materials/material.h"

#include <glm/glm.hpp>

class Shader;
class Texture;

struct MaterialProperties;

class PhongMaterial : public Material
{
protected:
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
  Texture *diffuseTexture;
  Texture *specularTexture;
  Texture *normalTexture;
  float shininess;
  glm::vec3 emissive;

public:
  PhongMaterial(
      glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
      Texture *diffuseTexture, Texture *specularTexture, Texture *normalTexture, float shininess, glm::vec3 emissive = glm::vec3(0.f));
  PhongMaterial(
      MaterialProperties material,
      Texture *diffuseTexture, Texture *specularTexture, Texture *normalTexture);
  ~PhongMaterial() = default;

  void sendToShader(Shader &program) override;
};
