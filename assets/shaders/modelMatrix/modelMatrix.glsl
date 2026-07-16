#ifndef MODEL_MATRIX_HELPER
#define MODEL_MATRIX_HELPER

void transform(vec3 instancePosition, vec4 orientation, vec3 scale, in vec3 position, out vec3 worldPosition)
{
  float xx = orientation.x * orientation.x;
  float yy = orientation.y * orientation.y;
  float zz = orientation.z * orientation.z;
  float xy = orientation.x * orientation.y;
  float xz = orientation.x * orientation.z;
  float yz = orientation.y * orientation.z;
  float wx = orientation.w * orientation.x;
  float wy = orientation.w * orientation.y;
  float wz = orientation.w * orientation.z;

  float r00 = 1.0 - 2.0 * (yy + zz);
  float r10 = 2.0 * (xy + wz);
  float r20 = 2.0 * (xz - wy);

  float r01 = 2.0 * (xy - wz);
  float r11 = 1.0 - 2.0 * (xx + zz);
  float r21 = 2.0 * (yz + wx);

  float r02 = 2.0 * (xz + wy);
  float r12 = 2.0 * (yz - wx);
  float r22 = 1.0 - 2.0 * (xx + yy);

  position *= scale;
  worldPosition.x = r00 * position.x + r01 * position.y + r02 * position.z;
  worldPosition.y = r10 * position.x + r11 * position.y + r12 * position.z;
  worldPosition.z = r20 * position.x + r21 * position.y + r22 * position.z;
  worldPosition += instancePosition;
}

void transform(vec3 instancePosition, vec4 orientation, vec3 scale, in vec3 position, in vec3 normal, out vec3 worldPosition, out vec3 worldNormal)
{
  float xx = orientation.x * orientation.x;
  float yy = orientation.y * orientation.y;
  float zz = orientation.z * orientation.z;
  float xy = orientation.x * orientation.y;
  float xz = orientation.x * orientation.z;
  float yz = orientation.y * orientation.z;
  float wx = orientation.w * orientation.x;
  float wy = orientation.w * orientation.y;
  float wz = orientation.w * orientation.z;

  float r00 = 1.0 - 2.0 * (yy + zz);
  float r10 = 2.0 * (xy + wz);
  float r20 = 2.0 * (xz - wy);

  float r01 = 2.0 * (xy - wz);
  float r11 = 1.0 - 2.0 * (xx + zz);
  float r21 = 2.0 * (yz + wx);

  float r02 = 2.0 * (xz + wy);
  float r12 = 2.0 * (yz - wx);
  float r22 = 1.0 - 2.0 * (xx + yy);

  position *= scale;
  worldPosition.x = r00 * position.x + r01 * position.y + r02 * position.z;
  worldPosition.y = r10 * position.x + r11 * position.y + r12 * position.z;
  worldPosition.z = r20 * position.x + r21 * position.y + r22 * position.z;
  worldPosition += instancePosition;

  normal /= scale;
  worldNormal.x = r00 * normal.x + r01 * normal.y + r02 * normal.z;
  worldNormal.y = r10 * normal.x + r11 * normal.y + r12 * normal.z;
  worldNormal.z = r20 * normal.x + r21 * normal.y + r22 * normal.z;
  worldNormal = normalize(worldNormal);
}

void transform(vec3 instancePosition, vec4 orientation, vec3 scale, in vec3 position, in vec3 normal, in vec3 tangent, out vec3 worldPosition, out vec3 worldNormal, out vec3 worldTangent)
{
  float xx = orientation.x * orientation.x;
  float yy = orientation.y * orientation.y;
  float zz = orientation.z * orientation.z;
  float xy = orientation.x * orientation.y;
  float xz = orientation.x * orientation.z;
  float yz = orientation.y * orientation.z;
  float wx = orientation.w * orientation.x;
  float wy = orientation.w * orientation.y;
  float wz = orientation.w * orientation.z;

  float r00 = 1.0 - 2.0 * (yy + zz);
  float r10 = 2.0 * (xy + wz);
  float r20 = 2.0 * (xz - wy);

  float r01 = 2.0 * (xy - wz);
  float r11 = 1.0 - 2.0 * (xx + zz);
  float r21 = 2.0 * (yz + wx);

  float r02 = 2.0 * (xz + wy);
  float r12 = 2.0 * (yz - wx);
  float r22 = 1.0 - 2.0 * (xx + yy);

  position *= scale;
  worldPosition.x = r00 * position.x + r01 * position.y + r02 * position.z;
  worldPosition.y = r10 * position.x + r11 * position.y + r12 * position.z;
  worldPosition.z = r20 * position.x + r21 * position.y + r22 * position.z;
  worldPosition += instancePosition;

  normal /= scale;
  worldNormal.x = r00 * normal.x + r01 * normal.y + r02 * normal.z;
  worldNormal.y = r10 * normal.x + r11 * normal.y + r12 * normal.z;
  worldNormal.z = r20 * normal.x + r21 * normal.y + r22 * normal.z;
  worldNormal = normalize(worldNormal);

  worldTangent.x = r00 * tangent.x + r01 * tangent.y + r02 * tangent.z;
  worldTangent.y = r10 * tangent.x + r11 * tangent.y + r12 * tangent.z;
  worldTangent.z = r20 * tangent.x + r21 * tangent.y + r22 * tangent.z;
  worldTangent = normalize(worldTangent);
}

#endif