#pragma once

#include "scene/shadow/shadow.h"
#include "graphics/shader.h"

#include <vector>
#include <glm/glm.hpp>

class PointShadow : public Shadow
{
protected:
  glm::vec3 lightPos;
  float nearPlane;
  float farPlane;
  float aspectRatio;

  void init() override;

public:
  PointShadow(const GLuint width, const GLuint height, glm::vec3 lightPos, float nearPlane, float farPlane);

  void bindShadowMapFBO() const override;
  void sendShadowMatrices(Shader *shadowShader) const;
  void bind(Shader& shader) const override;

  glm::vec3 getLightPos() const { return this->lightPos; };
  float getNearPlane() const { return this->nearPlane; };
  float getFarPlane() const { return this->farPlane; };
  float getAspectRatio() const { return this->aspectRatio; };

  void setLightPos(const glm::vec3 &pos) { this->lightPos = pos; };
};