#version 410

#include "ubo/pbr_point_light.glsl"
#include "ubo/camera.glsl"
#include "constants/constants.glsl"

layout (location = 0) out vec4 fs_color;
layout (location = 1) out vec4 fs_emissive;

in VS_OUT {
  vec3 vs_position;
  vec2 vs_texcoord;
  flat uint textureLayer;
} fs_in;

uniform sampler2DArray impostors;

void main()
{
  vec3 viewDir = normalize(vec3(camPosition) - fs_in.vs_position);

  vec3 right = vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
  vec3 up = vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);

  vec2 p = fs_in.vs_texcoord * 2.0 - 1.0;
  float r2 = dot(p, p);
  if (r2 > 1.0) discard;

  float z = sqrt(max(0.0, 1.0 - r2));
  vec3 normal = normalize(p.x * right + p.y * up + z * viewDir);

  vec4 texColor = texture(impostors, vec3(fs_in.vs_texcoord, fs_in.textureLayer));
  vec3 albedo = texColor.rgb;

  vec3 toLight = pbrPointLight.position - fs_in.vs_position;
  float distance = length(toLight);
  vec3 L = normalize(toLight);

  float NdotL = max(dot(normal, L), 0.0);

  float intensity;
  if (pbrPointLight.isAreaLight)
    intensity = pbrPointLight.luminosity / (4.0 * PI * PI * pbrPointLight.radius * pbrPointLight.radius);
  else
    intensity = pbrPointLight.luminosity / (4.0 * PI * distance * distance);

  vec3 radiance = pbrPointLight.color * intensity;

  vec3 diffuse = albedo * NdotL;

  float fresnel = pow(1.0 - max(dot(viewDir, normal), 0.0), 2.0);
  vec3 specular = vec3(0.15) * fresnel;

  vec3 finalColor = (diffuse + specular) * radiance;

  finalColor *= 0.00001;

  fs_color = vec4(finalColor, texColor.a);
  fs_emissive = vec4(0.0);
}