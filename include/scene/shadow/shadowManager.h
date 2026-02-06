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
class Shader;

class ShadowManager
{
private:
  GLuint dirUBO = 0;
  GLuint pointUBO = 0;

  std::unique_ptr<DirectionalShadow> directionalShadow;
  std::unique_ptr<PointShadow> pointShadow;

public:
  ShadowManager() = default;
  ~ShadowManager() = default;

  void updateDirUBO(int enabled = 1);
  void updatePointUBO(int enabled = 1);

  void maskDirUBO();
  void maskPointUBO();

  void bindDirShadowUBO(GLuint &programID);
  void bindPointShadowUBO(GLuint &programID);

  void bindDirShadow(Shader &shader, int unit);
  void bindPointShadow(Shader &shader, int unit);

  void bindDirShadowFBO();
  void bindPointShadowFBO();

  void unbindDirShadowFBO();
  void unbindPointShadowFBO();

  GLuint &getDirUBO() { return this->dirUBO; };
  GLuint &getPointUBO() { return this->pointUBO; };

  std::unique_ptr<DirectionalShadow> &getDirShadow() { return this->directionalShadow; };
  std::unique_ptr<PointShadow> &getPointShadow() { return this->pointShadow; };

  void addDirShadow(std::unique_ptr<DirectionalShadow> shadow);
  void addPointShadow(std::unique_ptr<PointShadow> shadow);

  void updatePointShadowLightPosition(glm::vec3 pos);
};