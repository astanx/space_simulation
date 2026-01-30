#version 410

layout (location = 0) in vec3 vertex_position;

#include "ubo/shadow.glsl"

void main()
{
  gl_Position = ModelMatrix * LightSpaceMatrix * vec4(vertex_position, 1.0);
}