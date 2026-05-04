#version 410

#include "gamma/gamma_correction.glsl"

out vec4 fs_color;
  
in vec2 vs_texcoord;

uniform sampler2D hdrColorBuffer;
uniform sampler2D hdrEmissiveBuffer;
uniform sampler2D bloomBlur;
uniform float exposure;
uniform float bloomPower;

vec3 reinhard(vec3 x) {
  return x / (x + 1.0);
}

vec3 RRTAndODTFit(vec3 v) {
  vec3 a = v * (v + 0.0245786) - 0.000090537;
  vec3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
  return a / b;
}

vec3 ACESFilm(vec3 x) {
  const float a = 2.51f;
  const float b = 0.03f;
  const float c = 2.43f;
  const float d = 0.59f;
  const float e = 0.14f;
  return clamp((x*(a*x + b)) / (x*(c*x + d) + e), 0.0, 1.0);
}

void main()
{             
  vec3 hdrColor = texture(hdrColorBuffer, vs_texcoord).rgb;
  vec3 hdrEmissive = texture(hdrEmissiveBuffer, vs_texcoord).rgb;
  vec3 bloomColor = texture(bloomBlur, vs_texcoord).rgb;

  vec3 color = hdrColor + hdrEmissive + bloomColor * bloomPower;
  color *= exposure;

  vec4 mapped = vec4(ACESFilm(color), 1.0);
  mapped = gammaCorrection(mapped);
  
  fs_color = mapped;
} 
