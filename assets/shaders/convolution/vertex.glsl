#version 410
layout (location = 0) in vec3 vs_position;

out vec3 vs_pos;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

void main()
{
  vs_pos = vs_position;  
  gl_Position = ProjectionMatrix * ViewMatrix * vec4(vs_position, 1.0);
}