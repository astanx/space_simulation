#ifndef INSTANCE_STRUCTS
#define INSTANCE_STRUCTS

typedef struct{
  float3 position;
  quat orientation;
  float3 scale;
} InstanceModelMatrixParts;

typedef struct{
  float3 position;
  float radius;
  uint textureLayer;
} InstancePositionRadiusTexture;

typedef struct{
  float3 position;
  float radius;
  float3 color;
} InstancePositionRadiusColor;

#endif