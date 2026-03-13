#version 410

out vec4 fs_color;
in vec3 vs_pos;

uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v)
{
  vec2 texcoord = vec2(atan(v.z, v.x), asin(v.y));
  texcoord *= invAtan;
  texcoord += 0.5;
  return texcoord;
}

void main()
{		
  vec2 texcoord = SampleSphericalMap(normalize(vs_pos));

  vec3 color = texture(equirectangularMap, texcoord).rgb;
    
  fs_color = vec4(color, 1.0);
}