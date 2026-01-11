#version 410

layout (location = 0) in vec3 vertex_position;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main()
{ 
  gl_Position = ProjectionMatrix * ViewMatrix * vec4(vertex_position, 1.f);
}