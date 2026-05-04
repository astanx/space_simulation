#version 410

out vec4 fs_color;
in vec2 vs_texcoord;

uniform sampler2D hdrColorBuffer;
uniform sampler2D hdrEmissiveBuffer;
uniform float threshold;

void main()
{            
  vec3 light = texture(hdrColorBuffer, vs_texcoord).rgb;
  vec3 emissive = texture(hdrEmissiveBuffer, vs_texcoord).rgb;

  vec3 color = light + emissive;

  float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));

  if (brightness > threshold)
    fs_color = vec4(color, 1.0);
  else
    fs_color = vec4(vec3(0.0), 1.0);
}