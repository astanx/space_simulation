
#version 330 core
out vec4 fs_color;
  
in vec2 vs_texcoord;

uniform sampler2D image;

const int MAX_KERNEL_SIZE = 40;
uniform int kernel_size;
  
uniform bool horizontal;
uniform float weight[MAX_KERNEL_SIZE];

void main()
{             
  vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
  vec3 result = texture(image, vs_texcoord).rgb * weight[0]; // current fragment's contribution
  if(horizontal)
  {
    for(int i = 1; i < kernel_size; ++i)
    {
    	result += texture(image, vs_texcoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
      result += texture(image, vs_texcoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
    }
  }
  else
  {
   	for(int i = 1; i < kernel_size; ++i)
    {
    	result += texture(image, vs_texcoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
      result += texture(image, vs_texcoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
    }
  }
	fs_color = vec4(result, 1.0);
}