#include "scene/shadow/shadowManager.h"
#include "scene/shadow/directionalShadow.h"
#include "scene/shadow/pointShadow.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// Public functions
void ShadowManager::updateDirUBO(const DirectionalShadow &dirShadow, int enabled)
{
  DirShadowGPU dirUBO{};

  dirUBO.ambient = glm::vec4(0);
  dirUBO.diffuse = glm::vec4(0);
  dirUBO.specular = glm::vec4(0);
  dirUBO.direction = glm::vec4(0);
  dirUBO.intensity = 1.0;
  dirUBO.enabled = enabled;

  glBindBuffer(GL_UNIFORM_BUFFER, this->dirUBO);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(DirShadowGPU), &dirUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ShadowManager::updatePointUBO(const PointShadow &pointShadow, int enabled)
{
  PointShadowGPU pointUBO{};

  float near = pointShadow.getNearPlane();
  float far = pointShadow.getFarPlane();
  glm::vec3 lightPos = pointShadow.getLightPos();
  glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), pointShadow.getAspectRatio(), near, far);

  pointUBO.ShadowMatrices[0] = shadowProj *
                               glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
  pointUBO.ShadowMatrices[1] = shadowProj *
                               glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
  pointUBO.ShadowMatrices[2] = shadowProj *
                               glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
  pointUBO.ShadowMatrices[3] = shadowProj *
                               glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
  pointUBO.ShadowMatrices[4] = shadowProj *
                               glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
  pointUBO.ShadowMatrices[5] = shadowProj *
                               glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));

  pointUBO.lightPos = glm::vec4(lightPos, 1.f);
  pointUBO.far_plane = far;

  std::cout << "Updating Point Shadow UBO with lightPos: " << lightPos.x << ", " << lightPos.y << ", " << lightPos.z << " and far_plane: " << far << std::endl;
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
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PointShadowGPU), &pointUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ShadowManager::maskDirUBO()
{
  this->updateDirUBO(DirectionalShadow(0, 0), 0);
}
void ShadowManager::maskPointUBO()
{
  this->updatePointUBO(PointShadow(0, 0, glm::vec3(0.0), 1.f, 100.f), 0);
}
void ShadowManager::bindDirShadow(GLuint &programID)
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