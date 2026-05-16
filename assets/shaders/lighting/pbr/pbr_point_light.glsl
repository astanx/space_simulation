#ifndef PBR_POINTLIGHT_STRUCT_GLSL
#define PBR_POINTLIGHT_STRUCT_GLSL

struct PBRPointLight
{
  vec3 position;
  float _pad0;

  vec3 color;
  float _pad1;

  float luminosity;
  float radius;
  bool isAreaLight;
  float _pad2;
};

#endif