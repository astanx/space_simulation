#version 410

#include "material/pbrMaterial.glsl"
#include "lighting/pbr/point_light.glsl"
#include "gamma/gamma_correction.glsl"
#include "shadow/point/point_shadow.glsl"

#include "ubo/dir_light.glsl"
#include "ubo/pbr_point_light.glsl"
#include "ubo/point_shadow.glsl"
#include "ubo/camera.glsl"

layout (location = 0) out vec4 fs_color;
layout (location = 1) out vec4 fs_emissive;

in VS_OUT {
  vec3 vs_position;
  vec2 vs_texcoord;
  vec3 vs_normal;

  mat3 TBN;

/*
  vec3 vs_tangentLightPos;
  vec3 vs_tangentCamPos;
  vec3 vs_tangentPos;
*/
} fs_in;

uniform PBRMaterial pbrMaterial;

uniform bool useTBN;

uniform samplerCube depthMap;
uniform samplerCube esmMap;
uniform samplerCube irradianceMap;

uniform bool useReflectorRadiance;
uniform samplerCube reflectorRadianceCubemap;
uniform vec3 reflectorPosition;

void main()
{
  PBRPointLight localPointLight = pbrPointLight;
  //PhongPointLight localPointLight = phongPointLight;

  vec3 normal = getNormal(pbrMaterial, fs_in.vs_texcoord, fs_in.vs_normal, useTBN);
  vec3 viewDir = normalize(camPosition.xyz - fs_in.vs_position);

  if (useTBN)
  { 
    mat3 TBN = fs_in.TBN;

    TBN[0] = normalize(TBN[0]);
    TBN[1] = normalize(TBN[1]);
    TBN[2] = normalize(TBN[2]);

    normal = normalize(TBN * normal);
  }
    
  vec4 albedo = getAlbedo(pbrMaterial, fs_in.vs_texcoord);

  if (albedo.a < 0.2) discard; // for atmosphere

  float shadow = CalcPointShadow(fs_in.vs_position, lightPos, depthMap, esmMap, far_plane, normal);

  MaterialData material = CalculateMaterial(pbrMaterial, fs_in.vs_texcoord);

  vec4 point = CalcPBRPointLight(normal, fs_in.vs_position, viewDir, material, localPointLight, shadow, irradianceMap, useReflectorRadiance, reflectorRadianceCubemap, reflectorPosition);
 
  vec4 result = point;

  fs_color = result;
  fs_emissive = vec4(material.emissive, 1.0);
}