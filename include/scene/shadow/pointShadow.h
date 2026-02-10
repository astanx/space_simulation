#pragma once

#include "scene/shadow/shadow.h"

#include <glm/glm.hpp>

class Shader;

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
  ~PointShadow() = default;

  void bindShadowMapFBO() const override;
  void bind(Shader &shader, int textureUnit) const override;

  glm::vec3 getLightPos() const { return this->lightPos; };
  float getNearPlane() const { return this->nearPlane; };
  float getFarPlane() const { return this->farPlane; };
  float getAspectRatio() const { return this->aspectRatio; };

  void setLightPos(const glm::vec3 &pos) { this->lightPos = pos; };
};