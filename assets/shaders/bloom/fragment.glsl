#version 410

out vec4 fs_color;
in vec2 vs_texcoord;

uniform sampler2D hdrBuffer;
uniform float threshold;

void main()
{            
  vec3 color = texture(hdrBuffer, vs_texcoord).rgb;
  float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));

  if (brightness > threshold)
    fs_color = vec4(color, 1.0);
  else
    fs_color = vec4(0.0, 0.0, 0.0, 1.0);
}