#ifndef PHONG_DIR_UBO_GLSL
#define PHONG_DIR_UBO_GLSL

#include "lighting/phong/dir_light.glsl"

layout(std140) uniform DirectionalLight 
{
  DirLight dirLight;
};

#endif