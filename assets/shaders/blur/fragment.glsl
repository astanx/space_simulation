#version 410

out vec4 fs_color;
  
in vec2 vs_texcoord;

uniform sampler2D image;
  
uniform bool horizontal;

uniform float offset[3] = float[](0.0, 1.3846153846, 3.2307692308);
uniform float weight[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

void main()
{             
  vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
  vec3 result = texture(image, vs_texcoord).rgb * weight[0]; // current fragment's contribution
  
  for(int i = 1; i < 3; ++i)
  {
    vec2 offsetVec;

    if (horizontal)
      offsetVec = vec2(tex_offset.x * offset[i], 0.0);
    else
      offsetVec = vec2(0.0, tex_offset.y * offset[i]);

    result += texture(image, vs_texcoord + offsetVec).rgb * weight[i];
    result += texture(image, vs_texcoord - offsetVec).rgb * weight[i];
  }

  fs_color = vec4(result, 1.0);
}