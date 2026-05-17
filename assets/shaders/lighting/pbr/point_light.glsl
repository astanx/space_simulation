#ifndef PBR_POINTLIGHT_GLSL
#define PBR_POINTLIGHT_GLSL

#include "lighting/pbr/point_light_helper.glsl"
#include "lighting/pbr/area_light.glsl"
#include "material/pbrMaterial.glsl"
#include "lighting/phong/point_light.glsl"
#include "lighting/pbr/pbr_point_light.glsl"

MaterialData CalculateMaterial(PBRMaterial material, vec2 texcoord)
{
  MaterialData m;

  m.albedo = getAlbedo(material, texcoord).rgb;
  m.roughness = getRoughness(material, texcoord);
  m.metallic = getMetallic(material, texcoord);
  m.ao = getAO(material, texcoord);

  vec3 night = getNight(material, texcoord).rgb;

  m.night = night;

  m.emissive = m.albedo * material.emissiveStrength;
  //m.emissive = night;

  return m;
}

vec4 CalcPBRPointLight(vec3 N, vec3 position, vec3 V, MaterialData material, PBRPointLight light, float shadow, samplerCube irradianceMap, vec3 reflectColor)
{
  vec3 F0 = vec3(0.04); 
  
  F0 = mix(F0, material.albedo, material.metallic);
	           
  // reflectance equation
  vec3 Lo = vec3(0.0);

  // calculate per-light radiance
  vec3 toLight = light.position - position;
  float distance = length(toLight);
  //if (distance > light.radius) return vec4(0.0);

  vec3 L = normalize(toLight);
  vec3 H = normalize(V + L);     
        
  // cook-torrance brdf
  float NDF = distributionGGX(N, H, material.roughness);        
  float G   = geometrySmith(N, V, L, material.roughness);      
  vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

  vec3 kS = F;
  vec3 kD = vec3(1.0) - kS;
  kD *= 1.0 - material.metallic;	

  float intensity;
  vec3 radiance;  

  if (light.isAreaLight)
  {
    intensity = light.luminosity / (4.0 * PI * light.radius * light.radius);

    radiance = light.color * intensity;

    vec3 specular = specular_evaluate(N, V, position, light, material.roughness);
    vec3 diffuse = diffuse_evaluate(N, V, position, light, material.albedo);
    Lo += (kD * diffuse + specular * kS) * radiance; 
  }
  else
  {
    intensity = light.luminosity / (4.0 * PI * distance * distance);

    radiance = light.color * intensity;  

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular     = numerator / denominator;

    // add to outgoing radiance Lo
    float NdotL = max(dot(N, L), 0.0);  
    Lo += (kD * material.albedo / PI + specular) * radiance * NdotL; 
  }

  Lo *= shadow;

  //vec3 Ro = material.ao * reflectColor * (kD * material.night + kS);
  vec3 Ro = reflectColor * kS;
  
  vec3 irradiance = texture(irradianceMap, N).rgb;
  //vec3 diffuse = irradiance * material.night;
  vec3 diffuse = irradiance * material.albedo;
  vec3 ambient = (kD * diffuse) * material.ao;

  vec3 color = ambient + Lo + Ro;

  return vec4(color, 1.0);
}

vec4 CalcPBRPointLight(vec3 N, vec3 position, vec3 V, MaterialData material, PBRPointLight light, float shadow, samplerCube irradianceMap)
{
  return CalcPBRPointLight(N, position, V, material, light, shadow, irradianceMap, vec3(0.0));
}

vec4 CalcReflectColor(vec3 N, vec3 position, samplerCube reflectorRadianceCubemap, vec3 reflectorPosition)
{
  vec3 fragToReflector = position - reflectorPosition;
  vec3 dir = normalize(fragToReflector);
  vec3 color = texture(reflectorRadianceCubemap, dir).rgb;

  return vec4(color, 1.0);
}

vec4 CalcPBRPointLight(vec3 N, vec3 position, vec3 V, MaterialData material, PBRPointLight light, float shadow, samplerCube irradianceMap, bool useReflectorRadiance, samplerCube reflectorRadianceCubemap, vec3 reflectorPosition)
{
  if (useReflectorRadiance)
  { 
    vec4 reflect = CalcReflectColor(N, position, reflectorRadianceCubemap, reflectorPosition);
    return CalcPBRPointLight(N, position, V, material, light, shadow, irradianceMap, reflect.xyz);
  }
  
  return CalcPBRPointLight(N, position, V, material, light, shadow, irradianceMap);
}

#endif
