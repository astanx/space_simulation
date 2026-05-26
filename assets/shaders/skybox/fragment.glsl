#version 410

#include "gamma/gamma_correction.glsl"

layout (location = 0) out vec4 fs_color;
layout (location = 1) out vec4 fs_emissive;

in vec3 vs_texcoord;

uniform samplerCube skybox;
uniform float exposure;

void main()
{    
	vec3 color = texture(skybox, vs_texcoord).xyz;

	//vec4 result = gammaCorrection(vec4(color, 1.0));

	fs_color = vec4(color, 1.0) / exposure;
}