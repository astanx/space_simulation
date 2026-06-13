#version 410

#include "ubo/camera.glsl"
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
} fs_in;

uniform PBRMaterial material;

uniform samplerCube depthMap;
uniform samplerCube esmMap;
uniform samplerCube irradianceMap;

void main()
{
  vec3 viewDir = normalize(camPosition.xyz - fs_in.vs_position);

  MaterialData m = CalculateMaterial(material, viewDir, fs_in.vs_normal, fs_in.vs_texcoord);

  vec4 point = CalcPBRPointLight(fs_in.vs_normal, fs_in.vs_position, viewDir, m, pbrPointLight, 1.0, irradianceMap);

  fs_color = point;
  fs_emissive = vec4(m.emissive, 1.0);
}
