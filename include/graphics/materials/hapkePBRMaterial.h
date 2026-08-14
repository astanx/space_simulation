#pragma once

#include "graphics/materials/pbrMaterial.h"

#include "physics/structs/hapkeParameters.h"

class HapkePBRMaterial : public PBRMaterial
{
private:
  HapkeParameters hapkeParameters;

public:
  HapkePBRMaterial(HapkeParameters hapke,
                   Texture *albedoMap,
                   Texture *normalMap,
                   Texture *aoMap,
                   Texture *metallicMap,
                   Texture *roughnessMap,
                   Texture *nightMap,
                   float emissiveStrength = 0.f,
                   float ao = 0.f,
                   float metallic = 0.f,
                   float roughness = 0.f);
  ~HapkePBRMaterial() = default;

  void sendToShader(Shader &program) override;
};