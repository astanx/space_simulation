#version 410

in vec4 vs_position;

layout(location = 0) out float fs_color;

#include "ubo/point_shadow.glsl"

void main()
{
  float lightDistance = length(vs_position.xyz - lightPos.xyz);
    
  lightDistance = lightDistance / far_plane;
    
  gl_FragDepth = lightDistance;

  float c = 40.0;

  fs_color = exp(-c * lightDistance);
}