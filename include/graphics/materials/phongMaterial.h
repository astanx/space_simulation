#pragma once

#include "graphics/materials/material.h"

#include <glm/glm.hpp>
#include <GL/glew.h>

class Shader;
class Texture;

class PhongMaterial : public Material
{
protected:
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
  Texture *diffuseTexture;
  Texture *specularTexture;
  float shininess;

public:
  PhongMaterial(
      glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
      Texture *diffuseTexture, Texture *specularTexture, float shininess);
  ~PhongMaterial() = default;

  void sendToShader(Shader &program) override;
};
