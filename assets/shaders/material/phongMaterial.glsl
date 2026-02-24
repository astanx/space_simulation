#ifndef PHONG_MATERIAL_GLSL
#define PHONG_MATERIAL_GLSL

#include "gamma/gamma_correction.glsl"

struct PhongMaterial
{
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  sampler2D diffuseTexture;
  sampler2D specularTexture;
  sampler2D normalTexture;
  float shininess;
  vec3 emissive;
};

vec3 getAlbedo(bool isTexture, PhongMaterial mat, vec2 texcoord, vec3 color)
{
  vec3 albedo = isTexture
  ? texture(mat.diffuseTexture, texcoord).rgb
  : color;

  return srgbToLinear(vec4(albedo, 1.0)).rgb;
}

vec3 getAlbedo(PhongMaterial mat, vec2 texcoord)
{
  vec3 albedo = texture(mat.diffuseTexture, texcoord).rgb;

  return srgbToLinear(vec4(albedo, 1.0)).rgb;
}

vec3 getSpecularMap(bool isTexture, PhongMaterial mat, vec2 texcoord)
{
  return isTexture
  ? texture(mat.specularTexture, texcoord).rgb
  : vec3(1.0);
}

vec3 getSpecularMap(PhongMaterial mat, vec2 texcoord)
{
  return texture(mat.specularTexture, texcoord).rgb;
}

#endif