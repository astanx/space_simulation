#ifndef DIR_UBO_GLSL
#define DIR_UBO_GLSL

#include "lighting/phong/dir_light.glsl"

layout(std140) uniform PhongDirectionalLight 
{
  DirLight dirLight;
};

#endif