#ifndef GAMMA_GLSL
#define GAMMA_GLSL

const float GAMMA = 2.2;

vec4 gammaCorrection(vec4 color)
{
  return vec4(pow(color.rgb, vec3(1.0/GAMMA)), color.a);
}

vec4 srgbToLinear(vec4 texture)
{
  return vec4(pow(texture.rgb, vec3(GAMMA)), texture.a);
}

#endif