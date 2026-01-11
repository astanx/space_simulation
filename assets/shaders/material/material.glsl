#ifndef MATERIAL_GLSL
#define MATERIAL_GLSL

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
  return isTexture
  ? texture(mat.diffuseTexture, texcoord).rgb
  : color;
}

vec3 getSpecularMap(bool isTexture, Material mat, vec2 texcoord)
{
  return isTexture
  ? texture(mat.specularTexture, texcoord).rgb
  : vec3(1.0);
}

#endif