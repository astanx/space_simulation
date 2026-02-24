#version 410

#include "material/pbrMaterial.glsl"
#include "lighting/pbr/point_light.glsl"
#include "gamma/gamma_correction.glsl"
#include "shadow/point/point_shadow.glsl"

#include "ubo/dir_light.glsl"
#include "ubo/pbr_point_light.glsl"
#include "ubo/point_shadow.glsl"
#include "ubo/camera.glsl"

out vec4 fs_color;

in VS_OUT {
  vec3 vs_position;
  vec2 vs_texcoord;
  vec3 vs_normal;

  vec3 vs_tangentLightPos;
  vec3 vs_tangentCamPos;
  vec3 vs_tangentPos;
} fs_in;

uniform PBRMaterial pbrMaterial;

uniform bool useTBN;

uniform samplerCube depthMap;

void main()
{
  PBRPointLight localPointLight = pbrPointLight;
  //PhongPointLight localPointLight = phongPointLight;

  vec3 normal = getNormal(pbrMaterial, fs_in.vs_texcoord, fs_in.vs_normal, useTBN);
  vec3 viewDir;
  vec3 position;

  if (useTBN)
  {
    position = fs_in.vs_tangentPos;
    viewDir = normalize(fs_in.vs_tangentCamPos - position);

    localPointLight.position = fs_in.vs_tangentLightPos;
  }
  else
  {
    position = fs_in.vs_position;
    viewDir = normalize(camPosition.xyz - position);
  }

  vec3 albedo = getAlbedo(pbrMaterial, fs_in.vs_texcoord);

  float shadow = CalcPointShadow(fs_in.vs_position, lightPos, depthMap, far_plane, fs_in.vs_normal);

  vec4 point = CalcPBRPointLight(normal, position, viewDir, fs_in.vs_texcoord, pbrMaterial, localPointLight);
 
  point *= (1.0 - shadow);

  vec4 result = point;

  //result += vec4(phongMaterial.emissive * albedo, 0.0);

  fs_color = result;
}