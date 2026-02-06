#include "scene/shadow/shadowManager.h"
#include "scene/shadow/directionalShadow.h"
#include "scene/shadow/pointShadow.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// Public functions
void ShadowManager::updateDirUBO(int enabled)
{
  DirShadowGPU ubo{};

  ubo.ambient = glm::vec4(0);
  ubo.diffuse = glm::vec4(0);
  ubo.specular = glm::vec4(0);
  ubo.direction = glm::vec4(0);
  ubo.intensity = 1.0;
  ubo.enabled = enabled;

  glBindBuffer(GL_UNIFORM_BUFFER, this->dirUBO);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(DirShadowGPU), &ubo);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ShadowManager::updatePointUBO(int enabled)
{
  PointShadowGPU ubo{};

  float near = this->pointShadow->getNearPlane();
  float far = this->pointShadow->getFarPlane();
  glm::vec3 lightPos = this->pointShadow->getLightPos();
  glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), pointShadow->getAspectRatio(), near, far);

  ubo.ShadowMatrices[0] = shadowProj *
                          glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
  ubo.ShadowMatrices[1] = shadowProj *
                          glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
  ubo.ShadowMatrices[2] = shadowProj *
                          glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
  ubo.ShadowMatrices[3] = shadowProj *
                          glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
  ubo.ShadowMatrices[4] = shadowProj *
                          glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
  ubo.ShadowMatrices[5] = shadowProj *
                          glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));

  ubo.lightPos = glm::vec4(lightPos, 1.f);
  ubo.far_plane = far;

  // std::cout << "Updating Point Shadow UBO with lightPos: " << lightPos.x << ", " << lightPos.y << ", " << lightPos.z << " and far_plane: " << far << std::endl;
  // std::cout << "Shadow Matrices: " << std::endl;
  // for (int i = 0; i < 6; ++i) {
  //   std::cout << "Matrix " << i << ": " << std::endl;
  //   const glm::mat4& mat = pointUBO.shadowMatrices[i];
  //   for (int row = 0; row < 4; ++row) {
  //     for (int col = 0; col < 4; ++col) {
  //       std::cout << mat[col][row] << " ";
  //     }
  //     std::cout << std::endl;
  //   }
  // }

  glBindBuffer(GL_UNIFORM_BUFFER, this->pointUBO);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PointShadowGPU), &ubo);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ShadowManager::maskDirUBO()
{
  // this->updateDirUBO(DirectionalShadow(0, 0), 0);
  this->updateDirUBO(0);
}
void ShadowManager::maskPointUBO()
{
  // this->updatePointUBO(PointShadow(0, 0, glm::vec3(0.0), 1.f, 100.f), 0);
  this->updatePointUBO(0);
}
void ShadowManager::bindDirShadowUBO(GLuint &programID)
{
  GLuint blockIndex =
      glGetUniformBlockIndex(programID, "DirectionalShadow");

  if (blockIndex != GL_INVALID_INDEX)
  {
    glUniformBlockBinding(programID, blockIndex, DIR_SHADOW_BINDING);
  }
  glBindBufferBase(GL_UNIFORM_BUFFER, DIR_SHADOW_BINDING, this->dirUBO);
}
void ShadowManager::bindPointShadowUBO(GLuint &programID)
{
  GLuint blockIndex =
      glGetUniformBlockIndex(programID, "PointShadowBuffer");

  if (blockIndex != GL_INVALID_INDEX)
  {
    glUniformBlockBinding(programID, blockIndex, POINT_SHADOW_BINDING);
  }
  glBindBufferBase(GL_UNIFORM_BUFFER, POINT_SHADOW_BINDING, this->pointUBO);
}

void ShadowManager::bindDirShadow(Shader &shader, int unit)
{
  this->directionalShadow->bind(shader, unit);
}
void ShadowManager::bindPointShadow(Shader &shader, int unit)
{
  this->pointShadow->bind(shader, unit);
}

void ShadowManager::bindDirShadowFBO()
{
  this->directionalShadow->bindShadowMapFBO();
}
void ShadowManager::bindPointShadowFBO()
{
  this->pointShadow->bindShadowMapFBO();
}

void ShadowManager::unbindDirShadowFBO()
{
  this->directionalShadow->unbindShadowMapFBO();
}
void ShadowManager::unbindPointShadowFBO()
{
  this->pointShadow->unbindShadowMapFBO();
}

void ShadowManager::addDirShadow(std::unique_ptr<DirectionalShadow> shadow)
{
  this->directionalShadow = std::move(shadow);
}
void ShadowManager::addPointShadow(std::unique_ptr<PointShadow> shadow)
{
  this->pointShadow = std::move(shadow);
}
void ShadowManager::updatePointShadowLightPosition(glm::vec3 pos)
{
  this->pointShadow->setLightPos(pos);
}
