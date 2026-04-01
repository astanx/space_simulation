#version 410

#include "ubo/pbr_point_light.glsl"

#include "constants/constants.glsl"
#include "reflector/reflector.glsl"

out vec4 fs_color;

in VS_OUT {
  vec3 vs_position;
  vec3 vs_normal;
} fs_in;

uniform float lightLuminocity;

uniform vec3 receiverPosition;

uniform HapkeParameters hapkeParameters;

void main()
{
  vec3 toLight = pbrPointLight.position - fs_in.vs_position;
  float distance = length(toLight);
  vec3 L = normalize(toLight);
  vec3 N = fs_in.vs_normal;
  vec3 V = normalize(receiverPosition - fs_in.vs_position);

  float E = lightLuminocity / (4.0 * PI * distance * distance);

  float color = hapkeBRDF(N, V, L, hapkeParameters) * E;

  if (isnan(color) || color < 0.0)
    color = 0.0;

  fs_color = vec4(vec3(color), 1.0);
}
