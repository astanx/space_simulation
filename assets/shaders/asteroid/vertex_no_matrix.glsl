#version 410

#include "ubo/camera.glsl"
#include "ubo/pbr_point_light.glsl"

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 vertex_texcoord;
layout (location = 2) in vec3 vertex_normal;
layout (location = 3) in vec3 instancePosition;
layout (location = 4) in float instanceRadius;

out VS_OUT {
  vec3 vs_position;
  vec2 vs_texcoord;
  vec3 vs_normal;
} vs_out;

void main()
{
  vs_out.vs_position = vertex_position * instanceRadius + instancePosition;
  vs_out.vs_texcoord = vertex_texcoord;
  
  vs_out.vs_normal = normalize(vertex_normal);
  
  gl_Position = ProjectionMatrix * ViewMatrix * vec4(vs_out.vs_position, 1.0);
}