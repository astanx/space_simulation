#ifndef DIRLIGHT_GLSL
#define DIRLIGHT_GLSL

#include "material/material.glsl"

struct DirLight {
  vec3 direction;
  float intensity;  
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};  

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, Material mat, vec3 albedo, vec3 specularMap)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), mat.shininess);
    // combine results
    vec3 ambient  = light.ambient  * albedo;
    vec3 diffuse  = light.diffuse  * diff * albedo;
    vec3 specular = light.specular * spec * specularMap;
    return (ambient + diffuse + specular) * light.intensity;
} 

#endif