#version 410

layout (location = 0) in vec3 vertex_position;

uniform mat4 ModelMatrix;

void main()
{
  gl_Position = ModelMatrix * vec4(vertex_position, 1.0);
}