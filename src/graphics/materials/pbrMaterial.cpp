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
    Texture *nightMap,
    float emissiveStrength,
    float ao,
    float metallic,
    float roughness)
{
  this->albedoMap = albedoMap;
  this->normalMap = normalMap;
  this->aoMap = aoMap;
  this->metallicMap = metallicMap;
  this->roughnessMap = roughnessMap;
  this->nightMap = nightMap;
  this->emissiveStrength = emissiveStrength;
  this->ao = ao;
  this->metallic = metallic;
  this->roughness = roughness;
}

// Public functions
void PBRMaterial::sendToShader(Shader &program)
{
  sendTexture(this->albedoMap, program, TextureBindingPoints::Diffuse, "pbrMaterial.albedoMap");
  sendTexture(this->normalMap, program, TextureBindingPoints::Normal, "pbrMaterial.normalMap");
  sendTexture(this->aoMap, program, TextureBindingPoints::AoMap, "pbrMaterial.aoMap");
  sendTexture(this->metallicMap, program, TextureBindingPoints::MetallicMap, "pbrMaterial.metallicMap");
  sendTexture(this->roughnessMap, program, TextureBindingPoints::RoughnessMap, "pbrMaterial.roughnessMap");
  sendTexture(this->nightMap, program, TextureBindingPoints::NightMap, "pbrMaterial.nightMap");

  program.set1f(this->emissiveStrength, "pbrMaterial.emissiveStrength");
  program.set1f(this->ao, "pbrMaterial.ao");
  program.set1f(this->metallic, "pbrMaterial.metallic");
  program.set1f(this->roughness, "pbrMaterial.roughness");

  if (this->normalMap)
    program.set1i(1, "useTBN");
  else
    program.set1i(0, "useTBN");

  if (this->nightMap)
    program.set1i(1, "pbrMaterial.useNightMap");
  else
    program.set1i(0, "pbrMaterial.useNightMap");
}