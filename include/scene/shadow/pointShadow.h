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

  std::unique_ptr<Texture> depthTexture;

  void init() override;
  void initShadowMap();
  void initDepthTexture();
  void initFBO();

public:
  PointShadow(const GLuint width, const GLuint height, glm::vec3 lightPos, float nearPlane, float farPlane);
  ~PointShadow() = default;

  void bind(Shader &shader, int textureUnit, const std::string &name = "esmMap") const override;
  void bindDepth(Shader &shader, int textureUnit, const std::string &name = "depthMap") const;

  glm::vec3 getLightPos() const { return this->lightPos; };
  float getNearPlane() const { return this->nearPlane; };
  float getFarPlane() const { return this->farPlane; };
  float getAspectRatio() const { return this->aspectRatio; };

  const Texture &getDepthTexture() const { return *this->depthTexture; }

  void setLightPos(const glm::vec3 &pos) { this->lightPos = pos; };
};