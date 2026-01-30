#version 410
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

#include "ubo/point_shadow.glsl"

out vec4 vs_position;

void main()
{
  for(int face = 0; face < 6; ++face)
  {
    gl_Layer = face;
    for(int i = 0; i < 3; ++i) 
    {
      vs_position = gl_in[i].gl_Position;
      gl_Position = ShadowMatrices[face] * vs_position;
      EmitVertex();
    }    
    EndPrimitive();
  }
} 