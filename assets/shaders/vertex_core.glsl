#version 410

#include "ubo/camera.glsl"
#include "ubo/pbr_point_light.glsl"

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 vertex_texcoord;
layout (location = 2) in vec3 vertex_normal;
layout (location = 3) in vec4 vertex_tangent;

out VS_OUT {
  vec3 vs_position;
  vec2 vs_texcoord;
  vec3 vs_normal;

  mat3 TBN;

/*
  vec3 vs_tangentLightPos;
  vec3 vs_tangentCamPos;
  vec3 vs_tangentPos;
*/
} vs_out;

uniform mat4 ModelMatrix;

uniform bool useTBN;

void main()
{
  vs_out.vs_position = vec3(ModelMatrix * vec4(vertex_position, 1.0));
  //vs_out.vs_texcoord = vec2(vertex_texcoord.x, vertex_texcoord.y * -1);
  vs_out.vs_texcoord = vec2(vertex_texcoord.x, vertex_texcoord.y);
  
  mat3 normalMatrix = transpose(inverse(mat3(ModelMatrix)));
  vs_out.vs_normal = normalize(normalMatrix * vertex_normal);

  if (useTBN)
  {
    vec3 N = vs_out.vs_normal;
    vec3 T = normalize(normalMatrix * vertex_tangent.xyz);

    // Gram-Schmidt re-orthogonalization
    T = normalize(T - dot(T, N) * N);

    vec3 B = cross(N, T) * vertex_tangent.w;

    vs_out.TBN = mat3(T, B, N);
  /*
    mat3 TBN = transpose(mat3(T, B, N));

    vs_out.vs_tangentLightPos = TBN * pbrPointLight.position;
    vs_out.vs_tangentCamPos  = TBN * camPosition.xyz;
    vs_out.vs_tangentPos  = TBN * vs_out.vs_position;
  */
  }

  gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertex_position, 1.f);
}