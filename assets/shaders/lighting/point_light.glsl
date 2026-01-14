#ifndef POINTLIGHT_GLSL
#define POINTLIGHT_GLSL

#include "material/material.glsl"

struct PointLight
{
  vec4 position;
  
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;

  float intensity;
  float constant;
  float linear;
  float quadratic;
};

vec4 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material mat, vec3 albedo, vec3 specularMap)
{
  vec3 lightDir = normalize(light.position.xyz - fragPos);
  float diff = max(dot(normal, lightDir), 0.0);

  vec3 halfwayDir = normalize(lightDir + viewDir);
  float spec = pow(max(dot(normal, halfwayDir), 0.0), mat.shininess);

  float distance = length(light.position.xyz - fragPos);
  float attenuation = 1.0 / (
    light.constant +
    light.linear * distance +
    light.quadratic * distance * distance
  );

  vec4 ambient  = light.ambient * vec4(albedo, 1.0);
  vec4 diffuse  = light.diffuse * diff * vec4(albedo, 1.0);
  vec4 specular = light.specular * spec * vec4(specularMap, 1.0);

  ambient  *= attenuation;
  diffuse  *= attenuation;
  specular *= attenuation;
  return (ambient + diffuse + specular) * light.intensity;
} 

#endif