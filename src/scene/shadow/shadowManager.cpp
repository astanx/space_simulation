#include "scene/shadow/shadowManager.h"

#include "debug/logger.h"

#include "scene/scene.h"
#include "scene/shadow/directionalShadow.h"
#include "scene/shadow/pointShadow.h"

#include "graphics/bindings/ubo.h"
#include "graphics/bindings/texture.h"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

// Private functions
void ShadowManager::initDirUBO()
{
  if (!glIsBuffer(this->dirUBO))
  {
    glGenBuffers(1, &this->dirUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, this->dirUBO);
    GL_CALL(glBufferData(GL_UNIFORM_BUFFER, sizeof(DirShadowGPU), nullptr, GL_DYNAMIC_DRAW));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }
}
void ShadowManager::initPointUBO()
{
  if (!glIsBuffer(this->pointUBO))
  {
    glGenBuffers(1, &this->pointUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, this->pointUBO);
    GL_CALL(glBufferData(GL_UNIFORM_BUFFER, sizeof(PointShadowGPU), nullptr, GL_DYNAMIC_DRAW));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }
}
// Constructor/Destructor
ShadowManager::ShadowManager(Scene &scene)
{
  if (scene.getDirLight())
    this->initDirUBO();
  if (!scene.getPointLights().empty())
    this->initPointUBO();
}
ShadowManager::~ShadowManager()
{
  if (glIsBuffer(this->dirUBO))
    glDeleteBuffers(1, &this->dirUBO);
  if (glIsBuffer(this->pointUBO))
    glDeleteBuffers(1, &this->pointUBO);
}

// Public functions
void ShadowManager::updateDirUBO(int enabled)
{
  if (this->directionalShadow && glIsBuffer(this->dirUBO) && enabled)
  {
    // todo directional shadows
    DirShadowGPU ubo{};

    ubo.ambient = glm::vec4(0);
    ubo.diffuse = glm::vec4(0);
    ubo.specular = glm::vec4(0);
    ubo.direction = glm::vec4(0);
    ubo.intensity = 1.0;
    ubo.enabled = enabled;

    glBindBuffer(GL_UNIFORM_BUFFER, this->dirUBO);
    GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(DirShadowGPU), &ubo));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }
  else
  {
    Logger::logWarning("Shadow manager", "Masking directional shadow UBO");
    this->maskDirUBO();
  }
}

void ShadowManager::updatePointUBO(int enabled)
{
  if (this->pointShadow && glIsBuffer(this->pointUBO) && enabled)
  {
    PointShadowGPU ubo{};

    float near = this->pointShadow->getNearPlane();
    float far = this->pointShadow->getFarPlane();
    glm::vec3 lightPos = this->pointShadow->getLightPos();
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), this->pointShadow->getAspectRatio(), near, far);

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
    GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PointShadowGPU), &ubo));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }
  else
  {
    Logger::logWarning("Shadow manager", "Masking point shadow UBO");
    this->maskPointUBO();
  }
}

void ShadowManager::maskDirUBO()
{
  if (!glIsBuffer(this->dirUBO))
  {
    Logger::logWarning("Shadow manager", "No directional UBO to mask");
    return;
  }

  DirShadowGPU ubo{};

  ubo.enabled = 0;
  glBindBuffer(GL_UNIFORM_BUFFER, this->dirUBO);
  GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(DirShadowGPU), &ubo));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void ShadowManager::maskPointUBO()
{
  if (!glIsBuffer(this->pointUBO))
  {
    Logger::logWarning("Shadow manager", "No point UBO to mask");
    return;
  }

  PointShadowGPU ubo{};
  // ubo.enabled = 0;
  glBindBuffer(GL_UNIFORM_BUFFER, this->pointUBO);
  GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PointShadowGPU), &ubo));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ShadowManager::bindDirShadowUBO(GLuint &programID)
{
  if (!glIsBuffer(this->dirUBO))
    Logger::logWarning("Shadow manager", "No dir UBO to bind");

  GLuint blockIndex =
      glGetUniformBlockIndex(programID, "DirectionalShadow");

  if (blockIndex != GL_INVALID_INDEX)
  {
    glUniformBlockBinding(programID, blockIndex, UBOBindingPoints::DirectionalShadow);
  }
  glBindBufferBase(GL_UNIFORM_BUFFER, UBOBindingPoints::DirectionalShadow, this->dirUBO);
}
void ShadowManager::bindPointShadowUBO(GLuint &programID)
{
  if (!glIsBuffer(this->pointUBO))
    Logger::logWarning("Shadow manager", "No point UBO to bind");

  GLuint blockIndex =
      glGetUniformBlockIndex(programID, "PointShadowBuffer");

  if (blockIndex != GL_INVALID_INDEX)
  {
    glUniformBlockBinding(programID, blockIndex, UBOBindingPoints::PointShadow);
  }
  glBindBufferBase(GL_UNIFORM_BUFFER, UBOBindingPoints::PointShadow, this->pointUBO);
}

void ShadowManager::bindDirShadow(Shader &shader)
{
  assert(this->directionalShadow && "[Shadow manager] ASSERT: No directional shadow to bind");

  if (!this->directionalShadow)
    return;

  this->directionalShadow->bind(shader, TextureBindingPoints::DirectionalShadow);
}
void ShadowManager::bindPointShadow(Shader &shader)
{
  assert(this->pointShadow && "[Shadow manager] ASSERT: No point shadow to bind");

  if (!this->pointShadow)
    return;

  this->pointShadow->bind(shader, TextureBindingPoints::PointShadow);
}

void ShadowManager::bindDirShadowFBO() const
{
  assert(this->directionalShadow && "[Shadow manager] ASSERT: No directional shadow to bind FBO");

  if (!this->directionalShadow)
    return;

  this->directionalShadow->bindShadowMapFBO();
}
void ShadowManager::bindPointShadowFBO() const
{
  assert(this->pointShadow && "[Shadow manager] ASSERT: No point shadow to bind FBO");

  if (!this->pointShadow)
    return;

  this->pointShadow->bindShadowMapFBO();
}

void ShadowManager::unbindDirShadowFBO() const
{
  assert(this->directionalShadow && "[Shadow manager] ASSERT: No directional shadow to unbind FBO");

  if (!this->directionalShadow)
    return;

  this->directionalShadow->unbindShadowMapFBO();
}
void ShadowManager::unbindPointShadowFBO() const
{
  assert(this->pointShadow && "[Shadow manager] ASSERT: No point shadow to unbind FBO");

  if (!this->pointShadow)
    return;

  this->pointShadow->unbindShadowMapFBO();
}

// Setters
void ShadowManager::addDirShadow(std::unique_ptr<DirectionalShadow> shadow)
{
  this->initDirUBO();
  this->directionalShadow = std::move(shadow);
}
void ShadowManager::addPointShadow(std::unique_ptr<PointShadow> shadow)
{
  this->initPointUBO();
  this->pointShadow = std::move(shadow);
}
void ShadowManager::updatePointShadowLightPosition(glm::vec3 pos)
{
  if (this->pointShadow)
    this->pointShadow->setLightPos(pos);
  else
    Logger::logWarning("Shadow manager", "Can not update point shadow light position: no point shadow set");
}

// Getters
DirectionalShadow *ShadowManager::getDirShadow() const
{
  if (!this->directionalShadow)
  {
    Logger::logWarning("Shadow manager", "No directional shadow");
    return nullptr;
  }
  return this->directionalShadow.get();
}

PointShadow *ShadowManager::getPointShadow() const
{
  if (!this->pointShadow)
  {
    Logger::logWarning("Shadow manager", "No point shadow");
    return nullptr;
  }
  return this->pointShadow.get();
}