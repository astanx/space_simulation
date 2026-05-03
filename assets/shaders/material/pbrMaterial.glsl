#ifndef PBR_MATERIAL_GLSL
#define PBR_MATERIAL_GLSL

#include "gamma/gamma_correction.glsl"

struct PBRMaterial
{
  sampler2D albedoMap;
  sampler2D normalMap;
  sampler2D aoMap;
  sampler2D metallicMap;
  sampler2D roughnessMap;
  float emissiveStrength;
};

vec4 getAlbedo(PBRMaterial material, vec2 texcoord)
{
  vec4 albedo = texture(material.albedoMap, texcoord).rgba;

  return srgbToLinear(albedo);
}

vec3 getNormal(PBRMaterial material, vec2 texcoord, vec3 normal, bool hasNormalMap)
{
  if (!hasNormalMap) return normalize(normal);

  vec3 N = texture(material.normalMap, texcoord).rgb;
  N = normalize(N * 2.0 - 1.0);

  return N;
}

float getRoughness(PBRMaterial material, vec2 texcoord)
{
  return 0.5;
  return texture(material.roughnessMap, texcoord).r;
}

float getMetallic(PBRMaterial material, vec2 texcoord)
{
  return 0.0;
  return texture(material.metallicMap, texcoord).r;
}

float getAO(PBRMaterial material, vec2 texcoord)
{
  return 0.4;
  return texture(material.aoMap, texcoord).r;
}

#endif