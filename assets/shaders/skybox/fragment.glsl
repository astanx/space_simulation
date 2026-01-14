#version 410

#include "gamma/gamma_correction.glsl"

out vec4 fs_color;

in vec3 vs_texcoord;

uniform samplerCube skybox;

void main()
{    
	fs_color = texture(skybox, vs_texcoord);
}