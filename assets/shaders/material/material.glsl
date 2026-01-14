#ifndef MATERIAL_GLSL
#define MATERIAL_GLSL

#include "gamma/gamma_correction.glsl"

struct Material
{
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  sampler2D diffuseTexture;
  sampler2D specularTexture;
  float shininess;
};

vec3 getAlbedo(bool isTexture, Material mat, vec2 texcoord, vec3 color)
{
  vec3 albedo = isTexture
  ? texture(mat.diffuseTexture, texcoord).rgb
  : color;

  return srgbToLinear(vec4(albedo, 1.0)).rgb;
}

vec3 getSpecularMap(bool isTexture, Material mat, vec2 texcoord)
{
  return isTexture
  ? texture(mat.specularTexture, texcoord).rgb
  : vec3(1.0);
}

#endif