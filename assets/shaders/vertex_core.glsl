#version 410

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_color;
layout (location = 2) in vec2 vertex_texcoord;
layout (location = 3) in vec3 vertex_normal;

out vec3 vs_position;
out vec3 vs_color;
out vec2 vs_texcoord;
out vec3 vs_normal;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

uniform bool useModelMatrix;

mat4 getModelMatrix()
{
  return useModelMatrix ?
  ModelMatrix 
  : mat4(1.0);
}

void main()
{
  mat4 model = getModelMatrix();
  vs_position = vec4(model * vec4(vertex_position, 1.f)).xyz;
  vs_color = vertex_color;
  vs_texcoord = vec2(vertex_texcoord.x, vertex_texcoord.y * -1);
  
  mat3 normalMatrix = transpose(inverse(mat3(model)));
  vs_normal = normalize(normalMatrix * vertex_normal);

  gl_Position = ProjectionMatrix * ViewMatrix * model * vec4(vertex_position, 1.f);
}