#include "graphics/materials/pbrMaterial.h"

#include "graphics/bindings/texture.h"

#include "graphics/shader.h"
#include "graphics/texture.h"

#include "physics/structs/materialProperties.h"

#include <iostream>

// Constructor and Destructor
PBRMaterial::PBRMaterial(
    Texture *albedoMap,
    Texture *normalMap,
    Texture *aoMap,
    Texture *metallicMap,
    Texture *roughnessMap,
    float emissiveStrength)
{
  this->albedoMap = albedoMap;
  this->normalMap = normalMap;
  this->aoMap = aoMap;
  this->metallicMap = metallicMap;
  this->roughnessMap = roughnessMap;
  this->emissiveStrength = emissiveStrength;
}

// Public functions
void PBRMaterial::sendToShader(Shader &program)
{
  sendTexture(this->albedoMap, program, TextureBindingPoints::Diffuse, "pbrMaterial.albedoMap");
  sendTexture(this->normalMap, program, TextureBindingPoints::Normal, "pbrMaterial.normalMap");
  sendTexture(this->aoMap, program, TextureBindingPoints::AoMap, "pbrMaterial.aoMap");
  sendTexture(this->metallicMap, program, TextureBindingPoints::MetallicMap, "pbrMaterial.metallicMap");
  sendTexture(this->roughnessMap, program, TextureBindingPoints::RoughnessMap, "pbrMaterial.roughnessMap");

  program.set1f(this->emissiveStrength, "pbrMaterial.emissiveStrength");

  if (this->normalMap)
    program.set1i(1, "useTBN");
  else
    program.set1i(0, "useTBN");
}