#version 410

#include "material/material.glsl"
#include "gamma/gamma_correction.glsl"
#include "ubo/dir_light.glsl"
#include "ubo/point_light.glsl"

#include "ubo/camera.glsl"

out vec4 fs_color;

in VS_OUT {
  vec3 vs_position;
  vec3 vs_color;
  vec2 vs_texcoord;
  vec3 vs_normal;
} fs_in;

uniform Material material;
uniform bool isTexture;

void main()
{
  vec3 normal = normalize(fs_in.vs_normal);
  vec3 viewDir = normalize(camPosition.xyz - fs_in.vs_position);

  vec3 albedo = getAlbedo(isTexture, material, fs_in.vs_texcoord, fs_in.vs_color);
  vec3 specularMap = getSpecularMap(isTexture, material, fs_in.vs_texcoord);

  vec4 result = CalcDirLight(dirLight, normal, viewDir, material, albedo, specularMap);
  
  result += CalcPointLight(
    pointLight,
    normal,
    fs_in.vs_position,
    viewDir,
    material,
    albedo,
    specularMap
  );

  fs_color = gammaCorrection(result);
}