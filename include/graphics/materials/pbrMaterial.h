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
  Texture *nightMap;
  float emissiveStrength;
  float ao;
  float metallic;
  float roughness;

public:
  PBRMaterial(Texture *albedoMap,
              Texture *normalMap,
              Texture *aoMap,
              Texture *metallicMap,
              Texture *roughnessMap,
              Texture *nightMap,
              float emissiveStrength = 0.f,
              float ao = 0.f,
              float metallic = 0.f,
              float roughness = 0.f);

  ~PBRMaterial() = default;

  void sendToShader(Shader &program) override;
};
