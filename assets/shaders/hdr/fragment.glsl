#version 410

#include "gamma/gamma_correction.glsl"

out vec4 fs_color;
  
in vec2 vs_texcoord;

uniform sampler2D hdrBuffer;
uniform float exposure;

vec3 reinhard(vec3 x) {
  return x / (x + 1.0);
}

vec3 RRTAndODTFit(vec3 v) {
  vec3 a = v * (v + 0.0245786) - 0.000090537;
  vec3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
  return a / b;
}

void main()
{             
  vec3 hdrColor = texture(hdrBuffer, vs_texcoord).rgb;
  
  hdrColor *= exposure;

  vec4 mapped = vec4(RRTAndODTFit(hdrColor), 1.0);
  mapped = gammaCorrection(mapped);
  
  fs_color = mapped;
} 