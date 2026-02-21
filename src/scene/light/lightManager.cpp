#include "scene/light/lightManager.h"

#include "debug/logger.h"

#include "graphics/bindings/ubo.h"

#include "graphics/state/scopedBuffer.h"

#include "scene/scene.h"

#include "scene/light/directionalLight.h"
#include "scene/light/pointLight.h"

// Private functions
void LightManager::initDirUBO()
{
  if (!this->dirUBO)
  {
    this->dirUBO = std::make_unique<Buffer>();

    ScopedBuffer uboScope(*this->dirUBO, GL_UNIFORM_BUFFER);
    GL_CALL(glBufferData(GL_UNIFORM_BUFFER, sizeof(DirLightGPU), nullptr, GL_DYNAMIC_DRAW));
  }
}
void LightManager::initPointUBO()
{
  if (!this->pointUBO)
  {
    this->pointUBO = std::make_unique<Buffer>();

    ScopedBuffer uboScope(*this->pointUBO, GL_UNIFORM_BUFFER);
    GL_CALL(glBufferData(GL_UNIFORM_BUFFER, sizeof(PointLightGPU), nullptr, GL_DYNAMIC_DRAW));
  }
}

// Constructor
LightManager::LightManager(Scene &scene)
{
  if (scene.getDirLight())
    this->initDirUBO();

  if (!scene.getPointLights().empty())
    this->initPointUBO();
}
// Public functions
void LightManager::updateDirUBO(const DirectionalLight *dirLight, int enabled)
{
  if (dirLight && this->dirUBO && enabled)
  {
    DirLightGPU ubo{};

    ubo.ambient = glm::vec4(dirLight->getAmbient(), 1.0);
    ubo.diffuse = glm::vec4(dirLight->getDiffuse(), 1.0);
    ubo.specular = glm::vec4(dirLight->getSpecular(), 1.0);
    ubo.direction = glm::vec4(dirLight->getDirection(), 0.0);
    ubo.intensity = dirLight->getIntensity();
    ubo.enabled = enabled;

    ScopedBuffer uboScope(*this->dirUBO, GL_UNIFORM_BUFFER);
    GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(DirLightGPU), &ubo));
  }
  else
  {
    Logger::logWarning("Light manager", "Masking directional light UBO");
    this->maskDirUBO();
  }
}

void LightManager::updatePointUBO(const PointLight *pointLight, int enabled)
{
  if (pointLight && this->pointUBO && enabled)
  {
    PointLightGPU ubo{};

    ubo.ambient = glm::vec4(pointLight->getAmbient(), 1.0);
    ubo.diffuse = glm::vec4(pointLight->getDiffuse(), 1.0);
    ubo.specular = glm::vec4(pointLight->getSpecular(), 1.0);
    ubo.position = glm::vec4(pointLight->getPosition(), 0.0);
    ubo.intensity = pointLight->getIntensity();
    ubo.quadratic = pointLight->getQuadratic();
    ubo.constant = pointLight->getConstant();
    ubo.linear = pointLight->getLinear();
    ubo.enabled = enabled;

    ScopedBuffer uboScope(*this->pointUBO, GL_UNIFORM_BUFFER);
    GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PointLightGPU), &ubo));
  }
  else
  {
    Logger::logWarning("Light manager", "Masking point UBO");
    this->maskPointUBO();
  }
}

void LightManager::maskDirUBO()
{
  if (!this->dirUBO)
  {
    Logger::logWarning("Light manager", "No directional UBO to mask");
    return;
  }

  DirLightGPU ubo{};

  ubo.enabled = 0;

  ScopedBuffer uboScope(*this->dirUBO, GL_UNIFORM_BUFFER);
  GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(DirLightGPU), &ubo));
}

void LightManager::maskPointUBO()
{
  if (!this->pointUBO)
  {
    Logger::logWarning("Light manager", "No point UBO to mask");
    return;
  }

  PointLightGPU ubo{};

  ubo.enabled = 0;

  ScopedBuffer uboScope(*this->pointUBO, GL_UNIFORM_BUFFER);
  GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PointLightGPU), &ubo));
}

// void LightManager::updateSSBO(std::vector<PointLight> &pointLights)
// {
//   std::vector<PointLightGPU> pointLightsSSBO;
//   for (auto &pointLight : pointLights)
//   {
//     PointLightGPU pointUBO{};

//     pointUBO.ambient = glm::vec4(pointLight.getAmbient(), 1.0);
//     pointUBO.diffuse = glm::vec4(pointLight.getDiffuse(), 1.0);
//     pointUBO.specular = glm::vec4(pointLight.getSpecular(), 1.0);
//     pointUBO.position = glm::vec4(pointLight.getPosition(), 0.0);
//     pointUBO.intensity = pointLight.getIntensity();
//     pointUBO.quadratic = pointLight.getQuadratic();
//     pointUBO.constant = pointLight.getConstant();
//     pointUBO.linear = pointLight.getLinear();
//     pointUBO.enabled = 1;

//     pointLightsSSBO.push_back(pointUBO);
//   }

//   glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->pointSSBO);
//   glBufferSubData(
//       GL_SHADER_STORAGE_BUFFER,
//       0,
//       sizeof(PointLightGPU) * pointLightsSSBO.size(),
//       pointLightsSSBO.data());
//   glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
// }

void LightManager::initDirLightUBOBinding(GLuint &programID)
{
  if (!this->dirUBO)
  {
    Logger::logWarning("Light manager", "No directional UBO to init binding");
    return;
  }

  GLuint blockIndex =
      glGetUniformBlockIndex(programID, "DirectionalLight");

  if (blockIndex != GL_INVALID_INDEX)
  {
    glUniformBlockBinding(programID, blockIndex, UBOBindingPoints::DirectionalLight);
    Logger::logInfo("Light manager", "Directional light UBO successfully binded");
  }
}
void LightManager::initPointLightUBOBinding(GLuint &programID)
{
  if (!this->pointUBO)
  {
    Logger::logWarning("Light manager", "No point UBO to init binding");
    return;
  }

  GLuint blockIndex =
      glGetUniformBlockIndex(programID, "PointLightBuffer");

  if (blockIndex != GL_INVALID_INDEX)
  {
    glUniformBlockBinding(programID, blockIndex, UBOBindingPoints::PointLight);
    Logger::logInfo("Light manager", "Point light UBO successfully binded");
  }
}

void LightManager::bindDirLightUBO()
{
  if (!this->dirUBO)
  {
    Logger::logWarning("Light manager", "No directional UBO to bind");
    return;
  }

  this->dirUBO->bindBufferBase(GL_UNIFORM_BUFFER, UBOBindingPoints::DirectionalLight);
}
void LightManager::bindPointLightUBO()
{
  if (!this->pointUBO)
  {
    Logger::logWarning("Light manager", "No point UBO to bind");
    return;
  }

  this->pointUBO->bindBufferBase(GL_UNIFORM_BUFFER, UBOBindingPoints::PointLight);
}

// void LightManager::bindPointLightSSBO(GLuint &programID)
// {
//   GLuint blockIndex =
//       glGetUniformBlockIndex(programID, "PointLightsBuffer");

//   if (blockIndex != GL_INVALID_INDEX)
//   {
//     glUniformBlockBinding(programID, blockIndex, POINT_LIGHT_BINDING);
//   }
//   glBindBufferBase(GL_SHADER_STORAGE_BUFFER, POINT_LIGHT_BINDING, this->pointSSBO);
// }