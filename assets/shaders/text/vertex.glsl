#version 410

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 vertex_texcoord;

uniform mat4 projection;
out vec2 vs_texcoord;

void main()
{
  gl_Position = projection * vec4(vertex_position.xy, 0.0, 1.0);
  vs_texcoord = vertex_texcoord;
}