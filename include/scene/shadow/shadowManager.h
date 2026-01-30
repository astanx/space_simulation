#pragma once

#include <GL/glew.h>
#include "glm/glm.hpp"
#include <vector>

// make dir shadow
struct DirShadowGPU
{
  glm::vec4 direction;
  glm::vec4 ambient;
  glm::vec4 diffuse;
  glm::vec4 specular;
  float intensity;
  int enabled;
  glm::vec2 _pad0;
};

// struct PointShadowGPU
// {
//   glm::vec3 lightPos;
//   float far_plane;
//   float near_plane;
//   float aspect_ratio;

//   int enabled;

//   float _pad0;
// };

struct PointShadowGPU
{
  glm::mat4 ShadowMatrices[6];
  glm::vec4 lightPos;
  float far_plane;
  glm::vec3 _pad0;
};

enum ShadowUBOBindingPoints
{
  DIR_SHADOW_BINDING = 3,
  POINT_SHADOW_BINDING = 4
};

class DirectionalShadow;
class PointShadow;

class ShadowManager
{
private:
  GLuint dirUBO = 0;
  GLuint pointUBO;

public:
  ShadowManager() = default;
  ~ShadowManager() = default;

  void updateDirUBO(const DirectionalShadow &dirShadow, int enabled = 1);
  void updatePointUBO(const PointShadow &pointShadow, int enabled = 1);

  void maskDirUBO();
  void maskPointUBO();

  void bindDirShadow(GLuint &programID);
  void bindPointShadowUBO(GLuint &programID);

  GLuint &getDirUBO() { return this->dirUBO; };
  GLuint &getPointUBO() { return this->pointUBO; };
};