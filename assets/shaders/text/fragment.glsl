
#version 410

in vec2 vs_texcoord;
out vec4 fs_color;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{    
  vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, vs_texcoord).r);
  fs_color = vec4(textColor, 1.0) * sampled;
} 