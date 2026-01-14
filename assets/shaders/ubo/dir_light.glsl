#ifndef DIR_UBO_GLSL
#define DIR_UBO_GLSL

#include "lighting/dir_light.glsl"

layout(std140) uniform DirectionalLight 
{
  DirLight dirLight;
};

#endif