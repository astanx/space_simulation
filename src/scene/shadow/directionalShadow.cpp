#include "scene/shadow/directionalShadow.h"

#include "debug/logger.h"

#include "graphics/shader.h"

#include <iostream>

// Private functions
void DirectionalShadow::init()
{
  glGenFramebuffers(1, &this->shadowMapFBO);
  glGenTextures(1, &this->shadowMapTexture);

  glBindTexture(GL_TEXTURE_2D, this->shadowMapTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
               this->shadowWidth, this->shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float borderColor[] = {1.0, 1.0, 1.0, 1.0};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

  glBindFramebuffer(GL_FRAMEBUFFER, this->shadowMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->shadowMapTexture, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    Logger::logError("Directional shadow", "Directional shadow map FBO is not complete");

  glBindTexture(GL_TEXTURE_2D, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
};

// Constructor
DirectionalShadow::DirectionalShadow(const GLuint width, const GLuint height) : Shadow(width, height)
{
  this->init();
};

// Public functions
void DirectionalShadow::bindShadowMapFBO() const
{
  if (glIsFramebuffer(this->shadowMapFBO))
    glBindFramebuffer(GL_FRAMEBUFFER, this->shadowMapFBO);
  else
    Logger::logWarning("Directional shadow", "Directional shadow map FBO is not initialized");
}

void DirectionalShadow::bind(Shader &shader, int textureUnit) const
{
  if (glIsTexture(this->shadowMapTexture))
  {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, this->shadowMapTexture);

    // change
    shader.set1i(textureUnit, "directionalShadowMap");
  }
  else
    Logger::logWarning("Directional shadow", "Directional shadow map texture is not initialized");
}