#version 410

#include "ubo/pbr_point_light.glsl"
#include "ubo/point_shadow.glsl"

#include "shadow/point/point_shadow.glsl"
#include "lighting/pbr/point_light.glsl"

#include "gamma/gamma_correction.glsl"

out vec4 fs_color;

in VS_OUT {
  vec3 vs_position;
  vec2 vs_texcoord;
  vec3 vs_normal;

  vec3 vs_tangentLightPos;
  vec3 vs_tangentCamPos;
  vec3 vs_tangentPos;
} fs_in;

uniform sampler2D diffuseTexture;

uniform PBRMaterial material;

uniform samplerCube depthMap;
uniform samplerCube esmMap;
uniform samplerCube irradianceMap;

void main()
{

  PBRPointLight localPointLight = pbrPointLight;

  vec3 position = fs_in.vs_tangentPos;
  vec3 viewDir = normalize(fs_in.vs_tangentCamPos - position);

  localPointLight.position = fs_in.vs_tangentLightPos;

  vec4 point = CalcPBRPointLight(fs_in.vs_normal, position, viewDir, fs_in.vs_texcoord, material, localPointLight, 1.0, irradianceMap);

  fs_color = point;
}
