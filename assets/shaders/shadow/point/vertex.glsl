#version 410

#include "modelMatrix/modelMatrix.glsl"

layout (location = 0) in vec3 vertex_position;
layout (location = 4) in vec3 instancePosition;
layout (location = 5) in vec4 instanceOrientation;
layout (location = 6) in vec3 instanceScale;

void main()
{
  vec3 p;
  transform(instancePosition, instanceOrientation, instanceScale, vertex_position, p);
  gl_Position = vec4(p, 1.0);
}