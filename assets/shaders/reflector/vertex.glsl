#version 410

#include "modelMatrix/modelMatrix.glsl"

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 4) in vec4 instancePosition;
layout (location = 5) in vec4 instanceOrientation;
layout (location = 6) in vec4 instanceScale;

out VS_OUT {
  vec3 vs_position;
  vec3 vs_normal;
} vs_out;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

void main()
{
  vec3 p;
  vec3 n;
  transform(instancePosition.xyz, instanceOrientation, instanceScale.xyz, p, n);
  vs_out.vs_position = p;

  vs_out.vs_normal = n;
  
  gl_Position = ProjectionMatrix * ViewMatrix * vec4(p, 1.f);
}