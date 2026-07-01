#version 410

#include "lighting/pbr/point_light.glsl
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
uniform samplerCube irradianceMap;

void main()
{
  vec2 p = fs_in.vs_texcoord * 2.0 - 1.0;
  float r2 = dot(p, p);
  if (r2 > 1.0) discard;

  float z = sqrt(max(0.0, 1.0 - r2));
  
  vec3 right = vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
  vec3 up = vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
  vec3 to = normalize(vec3(camPosition) - fs_in.vs_position);

  vec3 normal = normalize(p.x * right + p.y * up + z * to);

  vec4 texColor = texture(impostors, vec3(fs_in.vs_texcoord, fs_in.textureLayer));

  vec3 albedo = texColor.rgb;

  vec3 L = pbrPointLight.position - fs_in.vs_position;
  float dist = length(L);
  L /= dist;

  float NdotL = max(dot(normal, L), 0.0);

  float attenuation = pbrPointLight.luminosity / (4 * PI * PI * dist * dist);

  vec3 direct = albedo * pbrPointLight.color * attenuation * NdotL;

  vec3 ambient = texture(irradianceMap, normal).rgb * albedo;

  vec3 color = direct + ambient;
  color *= 0.0000001;

  fs_color = vec4(direct + ambient, texColor.a);
  fs_emissive = vec4(0.0);
}