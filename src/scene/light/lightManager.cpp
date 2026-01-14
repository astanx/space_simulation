#include "scene/light/lightManager.h"
#include "scene/light/directionalLight.h"
#include "scene/light/pointLight.h"

// Public functions
void LightManager::updateDirUBO(const DirectionalLight &dirLight)
{
  DirLightGPU dirUBO{};

  dirUBO.ambient = glm::vec4(dirLight.getAmbient(), 1.0);
  dirUBO.diffuse = glm::vec4(dirLight.getDiffuse(), 1.0);
  dirUBO.specular = glm::vec4(dirLight.getSpecular(), 1.0);
  dirUBO.direction = glm::vec4(dirLight.getDirection(), 0.0);
  dirUBO.intensity = dirLight.getIntensity();

  glBindBuffer(GL_UNIFORM_BUFFER, this->dirUBO);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(DirLightGPU), &dirUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void LightManager::updatePointUBO(const PointLight &pointLight)
{
  PointLightGPU pointUBO{};

  pointUBO.ambient = glm::vec4(pointLight.getAmbient(), 1.0);
  pointUBO.diffuse = glm::vec4(pointLight.getDiffuse(), 1.0);
  pointUBO.specular = glm::vec4(pointLight.getSpecular(), 1.0);
  pointUBO.position = glm::vec4(pointLight.getPosition(), 0.0);
  pointUBO.intensity = pointLight.getIntensity();
  pointUBO.quadratic = pointLight.getQuadratic();
  pointUBO.constant = pointLight.getConstant();
  pointUBO.linear = pointLight.getLinear();

  glBindBuffer(GL_UNIFORM_BUFFER, this->pointUBO);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PointLightGPU), &pointUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void LightManager::updateSSBO(std::vector<PointLight> &pointLights)
{
  std::vector<PointLightGPU> pointLightsSSBO;
  for (auto &pointLight : pointLights)
  {
    PointLightGPU pointUBO{};

    pointUBO.ambient = glm::vec4(pointLight.getAmbient(), 1.0);
    pointUBO.diffuse = glm::vec4(pointLight.getDiffuse(), 1.0);
    pointUBO.specular = glm::vec4(pointLight.getSpecular(), 1.0);
    pointUBO.position = glm::vec4(pointLight.getPosition(), 0.0);
    pointUBO.intensity = pointLight.getIntensity();
    pointUBO.quadratic = pointLight.getQuadratic();
    pointUBO.constant = pointLight.getConstant();
    pointUBO.linear = pointLight.getLinear();

    pointLightsSSBO.push_back(pointUBO);
  }

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->pointSSBO);
  glBufferSubData(
      GL_SHADER_STORAGE_BUFFER,
      0,
      sizeof(PointLightGPU) * pointLightsSSBO.size(),
      pointLightsSSBO.data());
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void LightManager::bindDirLight(GLuint &programID)
{
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
  GLuint blockIndex =
      glGetUniformBlockIndex(programID, "PointLightBuffer");

  if (blockIndex != GL_INVALID_INDEX)
  {
    glUniformBlockBinding(programID, blockIndex, POINT_LIGHT_BINDING);
  }
  glBindBufferBase(GL_UNIFORM_BUFFER, POINT_LIGHT_BINDING, this->pointUBO);
}

void LightManager::bindPointLightSSBO(GLuint &programID)
{
  GLuint blockIndex =
      glGetUniformBlockIndex(programID, "PointLightsBuffer");

  if (blockIndex != GL_INVALID_INDEX)
  {
    glUniformBlockBinding(programID, blockIndex, POINT_LIGHT_BINDING);
  }
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, POINT_LIGHT_BINDING, this->pointSSBO);
}