#include "graphics/materials/hapkePBRMaterial.h"

#include "graphics/shader.h"

#include "graphics/bindings/texture.h"

HapkePBRMaterial::HapkePBRMaterial(HapkeParameters hapke,
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
    : hapkeParameters(hapke), PBRMaterial(albedoMap, normalMap, aoMap, metallicMap, roughnessMap, nightMap, emissiveStrength, ao, metallic, roughness) {};

void HapkePBRMaterial::sendToShader(Shader &program)
{
  PBRMaterial::sendToShader(program);
  program.set1f(this->hapkeParameters.w, "hapkeParameters.w");
  program.set1f(this->hapkeParameters.theta, "hapkeParameters.theta");
  program.set1f(this->hapkeParameters.h, "hapkeParameters.h");
  program.set1f(this->hapkeParameters.b0, "hapkeParameters.b0");
  program.set1f(this->hapkeParameters.h_cb, "hapkeParameters.h_cb");
  program.set1f(this->hapkeParameters.b0_cb, "hapkeParameters.b0_cb");
  program.set1f(this->hapkeParameters.b, "hapkeParameters.b");
  program.set1f(this->hapkeParameters.c, "hapkeParameters.c");
}