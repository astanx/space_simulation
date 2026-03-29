#include "scene/shadow/shadowManager.h"

#include "debug/logger.h"

#include "scene/scene.h"
#include "scene/shadow/directionalShadow.h"
#include "scene/shadow/pointShadow.h"

#include "graphics/bindings/ubo.h"
#include "graphics/bindings/texture.h"

#include "graphics/state/scopedBuffer.h"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

// Private functions
void ShadowManager::initDirUBO()
{
  if (!this->dirUBO)
  {
    this->dirUBO = std::make_unique<Buffer>();

    ScopedBuffer uboScope(*this->dirUBO, GL_UNIFORM_BUFFER);
    GL_CALL(glBufferData(GL_UNIFORM_BUFFER, sizeof(DirShadowGPU), nullptr, GL_DYNAMIC_DRAW));
  }
}
void ShadowManager::initPointUBO()
{
  if (!this->pointUBO)
  {
    this->pointUBO = std::make_unique<Buffer>();

    ScopedBuffer uboScope(*this->pointUBO, GL_UNIFORM_BUFFER);
    GL_CALL(glBufferData(GL_UNIFORM_BUFFER, sizeof(PointShadowGPU), nullptr, GL_DYNAMIC_DRAW));
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

// Public functions
void ShadowManager::updateDirUBO(int enabled)
{
  if (this->directionalShadow && this->dirUBO && enabled)
  {
    // todo directional shadows
    DirShadowGPU ubo{};

    ubo.ambient = glm::vec4(0);
    ubo.diffuse = glm::vec4(0);
    ubo.specular = glm::vec4(0);
    ubo.direction = glm::vec4(0);
    ubo.intensity = 1.0;
    ubo.enabled = enabled;

    ScopedBuffer uboScope(*this->dirUBO, GL_UNIFORM_BUFFER);
    GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(DirShadowGPU), &ubo));
  }
  else
  {
    Logger::logWarning("Shadow manager", "Masking directional shadow UBO");
    this->maskDirUBO();
  }
}

void ShadowManager::updatePointUBO(int enabled)
{
  if (this->pointShadow && this->pointUBO && enabled)
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

    ScopedBuffer uboScope(*this->pointUBO, GL_UNIFORM_BUFFER);
    GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PointShadowGPU), &ubo));
  }
  else
  {
    Logger::logWarning("Shadow manager", "Masking point shadow UBO");
    this->maskPointUBO();
  }
}

void ShadowManager::maskDirUBO()
{
  if (!this->dirUBO)
  {
    Logger::logWarning("Shadow manager", "No directional UBO to mask");
    return;
  }

  DirShadowGPU ubo{};

  ubo.enabled = 0;

  ScopedBuffer uboScope(*this->dirUBO, GL_UNIFORM_BUFFER);
  GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(DirShadowGPU), &ubo));
}
void ShadowManager::maskPointUBO()
{
  if (!this->pointUBO)
  {
    Logger::logWarning("Shadow manager", "No point UBO to mask");
    return;
  }

  PointShadowGPU ubo{};
  // ubo.enabled = 0;

  ScopedBuffer uboScope(*this->pointUBO, GL_UNIFORM_BUFFER);
  GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PointShadowGPU), &ubo));
}

void ShadowManager::initDirShadowUBOBinding(GLuint &programID)
{
  if (!this->dirUBO)
  {
    Logger::logWarning("Shadow manager", "No dir UBO to init binding");
    return;
  }

  GLuint blockIndex =
      glGetUniformBlockIndex(programID, "DirectionalShadow");

  if (blockIndex != GL_INVALID_INDEX)
  {
    glUniformBlockBinding(programID, blockIndex, UBOBindingPoints::DirectionalShadow);
    Logger::logInfo("Shadow manager", "Directional shadow UBO successfully binded");
  }

  // no dir shadow supported rn
}
void ShadowManager::initPointShadowUBOBinding(GLuint &programID)
{
  if (!this->pointUBO)
  {
    Logger::logWarning("Shadow manager", "No point UBO to init binding");
    return;
  }

  GLuint blockIndex =
      glGetUniformBlockIndex(programID, "PointShadowBuffer");

  if (blockIndex != GL_INVALID_INDEX)
  {
    glUniformBlockBinding(programID, blockIndex, UBOBindingPoints::PointShadow);
    Logger::logInfo("Shadow manager", "Point shadow UBO successfully binded");
  }
}

void ShadowManager::bindDirShadowUBO()
{
  if (!this->dirUBO)
  {
    Logger::logWarning("Shadow manager", "No dir UBO to bind");
    return;
  }

  this->dirUBO->bindBufferBase(GL_UNIFORM_BUFFER, UBOBindingPoints::DirectionalShadow);
}
void ShadowManager::bindPointShadowUBO()
{
  if (!this->pointUBO)
  {
    Logger::logWarning("Shadow manager", "No point UBO to bind");
    return;
  }

  this->pointUBO->bindBufferBase(GL_UNIFORM_BUFFER, UBOBindingPoints::PointShadow);
}

void ShadowManager::bindDirShadow(Shader &shader, const std::string &name)
{
  assert(this->directionalShadow && "[Shadow manager] ASSERT: No directional shadow to bind");

  if (!this->directionalShadow)
    return;

  this->directionalShadow->bind(shader, TextureBindingPoints::DirectionalShadow, name);
}
void ShadowManager::bindPointShadow(Shader &shader, const std::string &name)
{
  assert(this->pointShadow && "[Shadow manager] ASSERT: No point shadow to bind");

  if (!this->pointShadow)
    return;

  this->pointShadow->bind(shader, TextureBindingPoints::PointShadow, name);
}

void ShadowManager::bindPointShadowDepth(Shader &shader, const std::string &name)
{
  assert(this->pointShadow && "[Shadow manager] ASSERT: No point shadow to bind depth");

  if (!this->pointShadow)
    return;

  this->pointShadow->bindDepth(shader, TextureBindingPoints::PointShadow, name);
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