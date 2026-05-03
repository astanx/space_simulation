#ifndef PBR_POINTLIGHT_GLSL
#define PBR_POINTLIGHT_GLSL

#include "lighting/pbr/point_light_helper.glsl"
#include "material/pbrMaterial.glsl"
#include "lighting/phong/point_light.glsl"

struct PBRPointLight
{
  vec3 position;
  float _pad0;

  vec3 color;
  float _pad1;

  float luminosity;
  float radius;
  vec2 _pad2;
};

vec4 CalcPBRPointLight(vec3 N, vec3 position, vec3 V, vec2 texcoord, PBRMaterial material, PBRPointLight light, float shadow, samplerCube irradianceMap, vec3 reflectColor)
{
  vec3 albedo = getAlbedo(material, texcoord).rgb;
  float roughness = getRoughness(material, texcoord);
  float metallic = getMetallic(material, texcoord);
  float ao = getAO(material, texcoord);

  vec3 F0 = vec3(0.04); 
  
  F0 = mix(F0, albedo, metallic);
	           
  // reflectance equation
  vec3 Lo = vec3(0.0);

  // calculate per-light radiance
  vec3 toLight = light.position - position;
  float distance = length(toLight);
  //if (distance > light.radius) return vec4(0.0);

  vec3 L = normalize(toLight);
  vec3 H = normalize(V + L);

  float intensity = light.luminosity / (4.0 * PI * distance * distance);

  vec3 radiance = light.color * intensity;        
        
  // cook-torrance brdf
  float NDF = distributionGGX(N, H, roughness);        
  float G   = geometrySmith(N, V, L, roughness);      
  vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
        
  vec3 kS = F;
  vec3 kD = vec3(1.0) - kS;
  kD *= 1.0 - metallic;	  
        
  vec3 numerator    = NDF * G * F;
  float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
  vec3 specular     = numerator / denominator;  
            
  // add to outgoing radiance Lo
  float NdotL = max(dot(N, L), 0.0);                
  Lo += (kD * albedo / PI + specular) * radiance * NdotL; 

  Lo *= shadow;

  vec3 Ro = ao * reflectColor * (kD * albedo + kS);
  
  vec3 emissive = albedo * material.emissiveStrength;

  vec3 irradiance = texture(irradianceMap, N).rgb;
  vec3 diffuse = irradiance * albedo;
  vec3 ambient = (kD * diffuse) * ao;
  
  vec3 color = ambient + Lo + Ro + emissive;

  return vec4(color, 1.0);
}

vec4 CalcPBRPointLight(vec3 N, vec3 position, vec3 V, vec2 texcoord, PBRMaterial material, PBRPointLight light, float shadow, samplerCube irradianceMap)
{
  return CalcPBRPointLight(N, position, V, texcoord, material, light, shadow, irradianceMap, vec3(0.0));
}

vec4 CalcReflectColor(vec3 N, vec3 position, samplerCube reflectorRadianceCubemap, vec3 reflectorPosition)
{
  vec3 fragToReflector = position - reflectorPosition;
  vec3 dir = normalize(fragToReflector);
  vec3 color = texture(reflectorRadianceCubemap, dir).rgb;

  return vec4(color, 1.0);
}

vec4 CalcPBRPointLight(vec3 N, vec3 tbn_position, vec3 V, vec2 texcoord, PBRMaterial material, PBRPointLight light, float shadow, samplerCube irradianceMap, bool useReflectorRadiance, samplerCube reflectorRadianceCubemap, vec3 reflectorPosition, vec3 normal, vec3 position)
{
  if (useReflectorRadiance)
  { 
    vec4 reflect = CalcReflectColor(normal, position, reflectorRadianceCubemap, reflectorPosition);
    return CalcPBRPointLight(N, tbn_position, V, texcoord, material, light, shadow, irradianceMap, reflect.xyz);
  }
  
  return CalcPBRPointLight(N, tbn_position, V, texcoord, material, light, shadow, irradianceMap);
}

#endif
