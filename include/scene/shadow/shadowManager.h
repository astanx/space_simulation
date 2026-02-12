#pragma once

#include "scene/shadow/pointShadow.h"
#include "scene/shadow/directionalShadow.h"

#include <memory>

#include <GL/glew.h>
#include <glm/glm.hpp>

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

class Shader;
class Scene;

class ShadowManager
{
private:
  GLuint dirUBO = 0;
  GLuint pointUBO = 0;

  std::unique_ptr<DirectionalShadow> directionalShadow;
  std::unique_ptr<PointShadow> pointShadow;

  void initDirUBO();
  void initPointUBO();

public:
  ShadowManager(Scene &scene);
  ShadowManager() = default;
  ~ShadowManager();

  void updateDirUBO(int enabled = 1);
  void updatePointUBO(int enabled = 1);

  void maskDirUBO();
  void maskPointUBO();

  void bindDirShadowUBO(GLuint &programID);
  void bindPointShadowUBO(GLuint &programID);

  void bindDirShadow(Shader &shader);
  void bindPointShadow(Shader &shader);

  void bindDirShadowFBO() const;
  void bindPointShadowFBO() const;

  void unbindDirShadowFBO() const;
  void unbindPointShadowFBO() const;

  DirectionalShadow *getDirShadow() const;
  PointShadow *getPointShadow() const;

  void addDirShadow(std::unique_ptr<DirectionalShadow> shadow);
  void addPointShadow(std::unique_ptr<PointShadow> shadow);

  void updatePointShadowLightPosition(glm::vec3 pos);
};