#ifndef POINT_UBO_GLSL
#define POINT_UBO_GLSL

#include "lighting/phong/point_light.glsl"

layout(std140) uniform PhongPointLightBuffer
{
	PointLight pointLight;
};

#endif 