#version 410

uniform sampler2D image;
uniform float filterRadius;

in vec2 vs_texcoord;
out vec4 fs_color;

void main()
{
  // The filter kernel is applied with a radius, specified in texture
  // coordinates, so that the radius will vary across mip resolutions.
  float x = filterRadius;
  float y = filterRadius;

  // Take 9 samples around current texel:
  // a - b - c
  // d - e - f
  // g - h - i
  // === ('e' is the current texel) ===
  vec3 a = texture(image, vec2(vs_texcoord.x - x, vs_texcoord.y + y)).rgb;
  vec3 b = texture(image, vec2(vs_texcoord.x,     vs_texcoord.y + y)).rgb;
  vec3 c = texture(image, vec2(vs_texcoord.x + x, vs_texcoord.y + y)).rgb;

  vec3 d = texture(image, vec2(vs_texcoord.x - x, vs_texcoord.y)).rgb;
  vec3 e = texture(image, vec2(vs_texcoord.x,     vs_texcoord.y)).rgb;
  vec3 f = texture(image, vec2(vs_texcoord.x + x, vs_texcoord.y)).rgb;

  vec3 g = texture(image, vec2(vs_texcoord.x - x, vs_texcoord.y - y)).rgb;
  vec3 h = texture(image, vec2(vs_texcoord.x,     vs_texcoord.y - y)).rgb;
  vec3 i = texture(image, vec2(vs_texcoord.x + x, vs_texcoord.y - y)).rgb;

  // Apply weighted distribution, by using a 3x3 tent filter:
  //  1   | 1 2 1 |
  // -- * | 2 4 2 |
  // 16   | 1 2 1 |
  vec3 upsample = e*4.0;
  upsample += (b+d+f+h)*2.0;
  upsample += (a+c+g+i);
  upsample *= 1.0 / 16.0;

  fs_color = vec4(upsample, 1.0);
}