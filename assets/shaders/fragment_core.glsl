#version 410

#include "material/phongMaterial.glsl"
#include "gamma/gamma_correction.glsl"
#include "shadow/point/point_shadow.glsl"
#include "ubo/phong_dir_light.glsl"
#include "ubo/phong_point_light.glsl"
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

uniform PhongMaterial material;

uniform bool useTBN;

uniform samplerCube depthMap;
//uniform bool isTexture;

void main()
{

  PointLight localPointLight = pointLight;

  vec3 normal;
  vec3 viewDir;
  vec3 position;

  if (useTBN)
  {
    normal = texture(material.normalTexture, fs_in.vs_texcoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    position = fs_in.vs_tangentPos;
    viewDir = normalize(fs_in.vs_tangentCamPos - position);

    localPointLight.position.xyz = fs_in.vs_tangentLightPos;
  }
  else
  {
    normal = normalize(fs_in.vs_normal);
    position = fs_in.vs_position;
    viewDir = normalize(camPosition.xyz - position);
  }

  vec3 albedo = getAlbedo(material, fs_in.vs_texcoord);
  vec3 specularMap = getSpecularMap(material, fs_in.vs_texcoord);
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
  
  float shadow = CalcPointShadow(fs_in.vs_position, lightPos, depthMap, far_plane, fs_in.vs_normal);

  vec4 point = CalcPointLight(
    localPointLight,
    normal,
    position,
    viewDir,
    material,
    albedo,
    specularMap,
    shadow
  );
 
  vec4 result = dir + point;

  result += vec4(material.emissive * albedo, 0.0);

  fs_color = result;
}