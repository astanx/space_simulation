#version 410

#include "material/material.glsl"
#include "gamma/gamma_correction.glsl"
#include "shadow/point/point_shadow.glsl"
#include "ubo/dir_light.glsl"
#include "ubo/point_light.glsl"
#include "ubo/point_shadow.glsl"

#include "ubo/camera.glsl"

out vec4 fs_color;

in VS_OUT {
  vec3 vs_position;
  vec3 vs_color;
  vec2 vs_texcoord;
  vec3 vs_normal;
} fs_in;

uniform Material material;

uniform samplerCube depthMap;
uniform bool isTexture;

void main()
{
  vec3 normal = normalize(fs_in.vs_normal);
  vec3 viewDir = normalize(camPosition.xyz - fs_in.vs_position);

  vec3 albedo = getAlbedo(isTexture, material, fs_in.vs_texcoord, fs_in.vs_color);
  vec3 specularMap = getSpecularMap(isTexture, material, fs_in.vs_texcoord);
/*

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

  */

  vec4 dir = CalcDirLight(dirLight, normal, viewDir, material, albedo, specularMap);
  
  float shadow = CalcPointShadow(fs_in.vs_position, lightPos, depthMap, far_plane, normal);

  vec4 point = CalcPointLight(
    pointLight,
    normal,
    fs_in.vs_position,
    viewDir,
    material,
    albedo,
    specularMap,
    shadow
  );

  vec4 result = dir + point;

  fs_color = gammaCorrection(result);
}