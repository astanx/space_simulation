#pragma once

#include "graphics/materials/material.h"

class Shader;
class Texture;

class AsteroidMaterial : public Material
{
protected:
  Texture *diffuseTexture;

public:
  AsteroidMaterial(Texture &diffuseTexture);
  ~AsteroidMaterial() = default;
  void sendToShader(Shader &shader) override;
};