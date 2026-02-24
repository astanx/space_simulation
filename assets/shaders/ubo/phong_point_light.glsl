#ifndef PHONG_POINT_UBO_GLSL
#define PHONG_POINT_UBO_GLSL

#include "lighting/phong/point_light.glsl"

layout(std140) uniform PhongPointLightBuffer
{
	PhongPointLight phongPointLight;
};

#endif 