#ifndef POINT_SSBO_GLSL
#define POINT_SSBO_GLSL

#include "lighting/point_light.glsl"

// Multiple-lights(not supported on opengl < 4.2)

layout(std430) buffer PointLightsBuffer
{
	PointLight pointLights[];
};

uniform int pointLightCount;

#endif 