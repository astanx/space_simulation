#ifndef PBR_POINT_UBO_GLSL
#define PBR_POINT_UBO_GLSL

#include "lighting/pbr/point_light.glsl"

layout(std140) uniform PBRPointLightBuffer
{
	PBRPointLight pbrPointLight;
};

#endif 