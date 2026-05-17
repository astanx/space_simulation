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

  // use averaged values
  float ao;
  float metallic;
  float roughness;

  float emissiveStrength;

  sampler2D nightMap;
  bool useNightMap;
  bool useRoughnessMap;
};

struct MaterialData
{
  vec3 albedo;
  vec3 night;
  float ao;
  float metallic;
  float roughness;
  vec3 emissive;
};

vec4 getAlbedo(PBRMaterial material, vec2 texcoord)
{
  vec4 albedo = texture(material.albedoMap, texcoord).rgba;

  return srgbToLinear(albedo);
}

vec4 getNight(PBRMaterial material, vec2 texcoord)
{
  if (!material.useNightMap)
    return vec4(vec3(0.0), 1.0);

  vec4 night = texture(material.nightMap, texcoord).rgba;

  return srgbToLinear(night);
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
  if (material.useRoughnessMap)
    return texture(material.roughnessMap, texcoord).r;

  return material.roughness;
}

float getMetallic(PBRMaterial material, vec2 texcoord)
{
  return material.metallic;
  return texture(material.metallicMap, texcoord).r;
}

float getAO(PBRMaterial material, vec2 texcoord)
{
  return material.ao;
  return texture(material.aoMap, texcoord).r;
}

#endif