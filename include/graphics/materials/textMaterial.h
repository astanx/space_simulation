#pragma once

#include "graphics/materials/material.h"

class Shader;
class Texture;

class TextMaterial : public Material
{
protected:
  Texture *diffuseTexture;

public:
  TextMaterial(Texture *diffuseTexture);
  ~TextMaterial() = default;
  void sendToShader(Shader &shader) override;
};