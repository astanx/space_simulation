#version 410

layout (location = 0) out vec4 fs_color;
layout (location = 1) out vec4 fs_emissive;

in VS_OUT {
  vec3 vs_color;
} fs_in;

void main()
{
  fs_color = vec4(fs_in.vs_color, 1.0);
  fs_emissive = vec4(vec3(0.0), 1.0);
}