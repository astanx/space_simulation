#version 410

#include "ubo/camera.glsl"

layout (location = 0) in vec4 instancePosition;
layout (location = 2) in vec4 instanceColor;
layout (location = 1) in float instanceRadius;

out VS_OUT {
  vec3 vs_color;
} vs_out;

void main()
{
  vs_out.vs_color = instanceColor.xyz;
  
  gl_Position = ProjectionMatrix * ViewMatrix * vec4(instancePosition.xyz, 1.f);

  gl_PointSize = instanceRadius;
  gl_PointSize = clamp(gl_PointSize, 1.0, 8.0);
}