#pragma once

#include "graphics/materials/material.h"

#include <glm/glm.hpp>

class Shader;
class Texture;

struct MaterialProperties;

class PBRMaterial : public Material
{
protected:
  Texture *albedoMap;
  Texture *normalMap;
  Texture *aoMap;
  Texture *metallicMap;
  Texture *roughnessMap;
  float emissiveStrength;

public:
  PBRMaterial(Texture *albedoMap,
              Texture *normalMap,
              Texture *aoMap,
              Texture *metallicMap,
              Texture *roughnessMap,
              float emissiveStrength = 0.0f);

  ~PBRMaterial() = default;

  void sendToShader(Shader &program) override;
};
