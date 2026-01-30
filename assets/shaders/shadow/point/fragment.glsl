#version 410

in vec4 vs_position;

#include "ubo/point_shadow.glsl"

void main()
{
  float lightDistance = length(vs_position.xyz - lightPos.xyz);
    
  lightDistance = lightDistance / far_plane;
    
  gl_FragDepth = lightDistance;
}