#ifndef POINTLIGHT_GLSL
#define POINTLIGHT_GLSL

#include "material/material.glsl"

struct PointLight
{
  vec3 position;
  float intensity;
  
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;
};

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material mat, vec3 albedo, vec3 specularMap)
{
  vec3 lightDir = normalize(light.position - fragPos);
  float diff = max(dot(normal, lightDir), 0.0);

  vec3 halfwayDir = normalize(lightDir + viewDir);
  float spec = pow(max(dot(normal, halfwayDir), 0.0), mat.shininess);

  float distance = length(light.position - fragPos);
  float attenuation = 1.0 / (
    light.constant +
    light.linear * distance +
    light.quadratic * distance * distance
  );

  vec3 ambient  = light.ambient * albedo;
  vec3 diffuse  = light.diffuse * diff * albedo;
  vec3 specular = light.specular * spec * specularMap;

  ambient  *= attenuation;
  diffuse  *= attenuation;
  specular *= attenuation;
  return (ambient + diffuse + specular) * light.intensity;
} 

#endif