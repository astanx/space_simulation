#ifndef POINT_UBO_GLSL
#define POINT_UBO_GLSL

#include "lighting/point_light.glsl"

layout(std140) uniform PointLightBuffer
{
	PointLight pointLight;
};

#endif 