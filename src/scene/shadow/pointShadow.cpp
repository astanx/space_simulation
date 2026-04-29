#include "scene/shadow/pointShadow.h"

#include "debug/logger.h"

#include "graphics/state/scopedFramebuffer.h"
#include "graphics/state/scopedTexture.h"

#include "graphics/shader.h"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

// Private functions
void PointShadow::init()
{
  this->initDepthTexture();
  this->initShadowMap();
  this->initFBO();
};

void PointShadow::initShadowMap()
{
  ScopedTexture shadowMap(*this->shadowMapTexture);

  for (unsigned i = 0; i < 6; i++)
    GL_CALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_R32F,
                         this->shadowWidth, this->shadowHeight, 0, GL_RED, GL_FLOAT, NULL));

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}
void PointShadow::initDepthTexture()
{
  ScopedTexture depth(*this->depthTexture);

  for (unsigned i = 0; i < 6; ++i)
    GL_CALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                         GL_DEPTH_COMPONENT32F, shadowWidth, shadowHeight, 0,
                         GL_DEPTH_COMPONENT, GL_FLOAT, nullptr));

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
void PointShadow::initFBO()
{
  ScopedFramebuffer shadowMap(*this->shadowMapFBO, GL_FRAMEBUFFER);

  GL_CALL(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->depthTexture->getId(), 0));
  GL_CALL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->shadowMapTexture->getId(), 0));

  glDrawBuffer(GL_COLOR_ATTACHMENT0);

  this->shadowMapFBO->checkComplete();
}

// Constructor
PointShadow::PointShadow(const GLuint width, const GLuint height, glm::vec3 lightPos, float nearPlane, float farPlane) : Shadow(width, height)
{
  this->lightPos = lightPos;
  this->nearPlane = nearPlane;
  this->farPlane = farPlane;
  this->aspectRatio = static_cast<float>(width) / static_cast<float>(height);

  this->shadowMapTexture = std::make_unique<Texture>(GL_TEXTURE_CUBE_MAP);
  this->depthTexture = std::make_unique<Texture>(GL_TEXTURE_CUBE_MAP);

  this->init();
};

// Public functions
void PointShadow::bind(Shader &shader, int textureUnit, const std::string &name) const
{
  const std::string &finalName = name.empty() ? "esmMap" : name;


  this->shadowMapTexture->activate(textureUnit);
  this->shadowMapTexture->bind();
  shader.set1i(textureUnit, finalName);
}

void PointShadow::bindDepth(Shader &shader, int textureUnit, const std::string &name) const
{
  const std::string &finalName = name.empty() ? "depthMap" : name;

  this->depthTexture->activate(textureUnit);
  this->depthTexture->bind();
  shader.set1i(textureUnit, finalName);
}