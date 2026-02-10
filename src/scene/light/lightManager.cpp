#include "scene/light/lightManager.h"

#include "debug/logger.h"

#include "scene/scene.h"
#include "scene/light/directionalLight.h"
#include "scene/light/pointLight.h"

// Private functions
void LightManager::initDirUBO()
{
  if (!glIsBuffer(this->dirUBO))
  {
    glGenBuffers(1, &this->dirUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, this->dirUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(DirLightGPU), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }
}
void LightManager::initPointUBO()
{
  if (!glIsBuffer(this->pointUBO))
  {

    glGenBuffers(1, &this->pointUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, this->pointUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PointLightGPU), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
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
  if (dirLight && glIsBuffer(this->dirUBO) && enabled)
  {
    DirLightGPU ubo{};

    ubo.ambient = glm::vec4(dirLight->getAmbient(), 1.0);
    ubo.diffuse = glm::vec4(dirLight->getDiffuse(), 1.0);
    ubo.specular = glm::vec4(dirLight->getSpecular(), 1.0);
    ubo.direction = glm::vec4(dirLight->getDirection(), 0.0);
    ubo.intensity = dirLight->getIntensity();
    ubo.enabled = enabled;

    glBindBuffer(GL_UNIFORM_BUFFER, this->dirUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(DirLightGPU), &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }
  else
  {
    Logger::logWarning("Light manager", "Masking directional light UBO");
    this->maskDirUBO();
  }
}

void LightManager::updatePointUBO(const PointLight *pointLight, int enabled)
{
  if (pointLight && glIsBuffer(this->pointUBO) && enabled)
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

    glBindBuffer(GL_UNIFORM_BUFFER, this->pointUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PointLightGPU), &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }
  else
  {
    Logger::logWarning("Light manager", "Masking point UBO");
    this->maskPointUBO();
  }
}

void LightManager::maskDirUBO()
{
  DirLightGPU ubo{};

  ubo.enabled = 0;

  glBindBuffer(GL_UNIFORM_BUFFER, this->dirUBO);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(DirLightGPU), &ubo);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void LightManager::maskPointUBO()
{
  PointLightGPU ubo{};

  ubo.enabled = 0;

  glBindBuffer(GL_UNIFORM_BUFFER, this->pointUBO);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PointLightGPU), &ubo);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
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

void LightManager::bindDirLight(GLuint &programID)
{
  if (!glIsBuffer(this->dirUBO))
    Logger::logWarning("Light manager", "No directional UBO to bind");

  GLuint blockIndex =
      glGetUniformBlockIndex(programID, "DirectionalLight");

  if (blockIndex != GL_INVALID_INDEX)
  {
    glUniformBlockBinding(programID, blockIndex, DIR_LIGHT_BINDING);
  }
  glBindBufferBase(GL_UNIFORM_BUFFER, DIR_LIGHT_BINDING, this->dirUBO);
}
void LightManager::bindPointLightUBO(GLuint &programID)
{
  if (!glIsBuffer(this->pointUBO))
    Logger::logWarning("Light manager", "No point UBO to bind");

  GLuint blockIndex =
      glGetUniformBlockIndex(programID, "PointLightBuffer");

  if (blockIndex != GL_INVALID_INDEX)
  {
    glUniformBlockBinding(programID, blockIndex, POINT_LIGHT_BINDING);
  }
  glBindBufferBase(GL_UNIFORM_BUFFER, POINT_LIGHT_BINDING, this->pointUBO);
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