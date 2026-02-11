#include "scene/shadow/pointShadow.h"

#include "debug/logger.h"

#include "graphics/shader.h"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

// Private functions
void PointShadow::init()
{
  glGenFramebuffers(1, &this->shadowMapFBO);
  glGenTextures(1, &this->shadowMapTexture);

  glBindTexture(GL_TEXTURE_CUBE_MAP, this->shadowMapTexture);

  for (unsigned i = 0; i < 6; i++)
    GL_CALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                         this->shadowWidth, this->shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glBindFramebuffer(GL_FRAMEBUFFER, this->shadowMapFBO);
  GL_CALL(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->shadowMapTexture, 0));
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    Logger::logError("Point shadow", "Point shadow map FBO is not complete");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
};

// Constructor
PointShadow::PointShadow(const GLuint width, const GLuint height, glm::vec3 lightPos, float nearPlane, float farPlane) : Shadow(width, height)
{
  this->lightPos = lightPos;
  this->nearPlane = nearPlane;
  this->farPlane = farPlane;
  this->aspectRatio = static_cast<float>(width) / static_cast<float>(height);

  this->init();
};

// Public functions
void PointShadow::bindShadowMapFBO() const
{
  if (glIsFramebuffer(this->shadowMapFBO))
  {
    glBindFramebuffer(GL_FRAMEBUFFER, this->shadowMapFBO);
    GL_CALL(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->shadowMapTexture, 0));
  }
  else
    Logger::logWarning("Point shadow", "Point shadow map FBO is not initialized");

  // glDrawBuffer(GL_NONE);
  // glReadBuffer(GL_NONE);
}

void PointShadow::bind(Shader &shader, int textureUnit) const
{
  if (glIsTexture(this->shadowMapTexture))
  {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->shadowMapTexture);
    shader.set1i(textureUnit, "depthMap");
  }
  else
    Logger::logWarning("Point shadow", "Point shadow map texture is not initialized");
}