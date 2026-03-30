#version 410

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;

out VS_OUT {
  vec3 vs_position;
  vec3 vs_normal;
} vs_out;

uniform mat4 ModelMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

void main()
{
  vs_out.vs_position = vec3(ModelMatrix * vec4(vertex_position, 1.0));

  mat3 normalMatrix = transpose(inverse(mat3(ModelMatrix)));
  vs_out.vs_normal = normalize(normalMatrix * vertex_normal);
  
  gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertex_position, 1.f);
}