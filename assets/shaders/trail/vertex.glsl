#version 410

#include "ubo/camera.glsl"

layout (location = 0) in vec3 vertex_position;

void main()
{ 
  gl_Position = ProjectionMatrix * ViewMatrix * vec4(vertex_position, 1.f);
}