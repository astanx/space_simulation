#version 410

#include "lighting/dir_light.glsl"

out vec4 fs_color;

in VS_OUT {
    vec3 vs_position;
    vec3 vs_normal;
    vec2 vs_texcoord;
} fs_in;

uniform sampler2D diffuseTexture;
uniform DirLight dirLight;

void main()
{
  fs_color = texture(diffuseTexture, fs_in.vs_texcoord);
}
