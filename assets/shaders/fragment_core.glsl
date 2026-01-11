#version 410

#include "material/material.glsl"
#include "lighting/dir_light.glsl"
#include "lighting/point_light.glsl"

out vec4 fs_color;

in VS_OUT {
  vec3 vs_position;
  vec3 vs_color;
  vec2 vs_texcoord;
  vec3 vs_normal;
} fs_in;

uniform Material material;
uniform PointLight pointLight;
uniform DirLight dirLight;
uniform bool isTexture;
uniform vec3 camPosition; 

void main()
{
  vec3 normal = normalize(fs_in.vs_normal);
  vec3 viewDir = normalize(camPosition - fs_in.vs_position);

  vec3 albedo = getAlbedo(isTexture, material, fs_in.vs_texcoord, fs_in.vs_color);
  vec3 specularMap = getSpecularMap(isTexture, material, fs_in.vs_texcoord);

  vec3 result = CalcDirLight(dirLight, normal, viewDir, material, albedo, specularMap);
  result += CalcPointLight(pointLight, normal, fs_in.vs_position, viewDir, material, albedo, specularMap);
  fs_color = vec4(result, 1.0);
}