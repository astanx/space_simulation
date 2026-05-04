#version 410

#include "ubo/pbr_point_light.glsl"
#include "ubo/point_shadow.glsl"

#include "shadow/point/point_shadow.glsl"
#include "lighting/pbr/point_light.glsl"

#include "gamma/gamma_correction.glsl"

layout (location = 0) out vec4 fs_color;
layout (location = 1) out vec4 fs_emissive;

in VS_OUT {
  vec3 vs_position;
  vec2 vs_texcoord;
  vec3 vs_normal;

  vec3 vs_tangentLightPos;
  vec3 vs_tangentCamPos;
  vec3 vs_tangentPos;
} fs_in;

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

  MaterialData m = CalculateMaterial(material, fs_in.vs_texcoord);

  vec4 point = CalcPBRPointLight(fs_in.vs_normal, position, viewDir, m, localPointLight, 1.0, irradianceMap);

  fs_color = point;
  fs_emissive = vec4(m.emissive, 1.0);
}
