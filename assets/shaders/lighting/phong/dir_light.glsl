#ifndef DIRLIGHT_GLSL
#define DIRLIGHT_GLSL

#include "material/phongMaterial.glsl"

struct DirLight {
  vec4 direction;
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;

  float intensity;  
  int enabled;
  vec2 _pad0;
};  

vec4 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, PhongMaterial mat, vec3 albedo, vec3 specularMap)
{
  if (light.enabled == 0) return vec4(0.0);

  vec3 lightDir = normalize(-light.direction.xyz);
  // diffuse shading
  float diff = max(dot(normal, lightDir), 0.0);
  // specular shading
  vec3 halfwayDir = normalize(lightDir + viewDir);
  float spec = pow(max(dot(normal, halfwayDir), 0.0), mat.shininess);
  // combine results
  vec4 ambient  = light.ambient  * vec4(albedo, 1.0);
  vec4 diffuse  = light.diffuse  * diff * vec4(albedo, 1.0);
  vec4 specular = light.specular * spec * vec4(specularMap, 1.0);
  return (ambient + diffuse + specular) * light.intensity;
} 

#endif