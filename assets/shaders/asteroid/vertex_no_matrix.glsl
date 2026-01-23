#version 410

#include "ubo/camera.glsl"

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 vertex_texcoord;
layout (location = 2) in vec3 vertex_normal;
layout (location = 3) in vec3 instancePosition;

out VS_OUT {
  vec3 vs_position;
  vec2 vs_texcoord;
  vec3 vs_normal;
} vs_out;

void main()
{
  vs_out.vs_position = vertex_position + instancePosition;
  vs_out.vs_texcoord = vec2(vertex_texcoord.x, vertex_texcoord.y * -1);
  
  vs_out.vs_normal = normalize(vertex_normal);
  
  gl_Position = ProjectionMatrix * ViewMatrix * vec4(vertex_position + instancePosition, 1.f);
}