#include "scene/shadow/directionalShadow.h"

#include "debug/logger.h"

#include "graphics/state/scopedFramebuffer.h"
#include "graphics/state/scopedTexture.h"

#include "graphics/shader.h"

#include <iostream>

// Private functions
void DirectionalShadow::init()
{
  this->shadowMapTexture->bind();

  GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                       this->shadowWidth, this->shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  float borderColor[] = {1.0, 1.0, 1.0, 1.0};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

  {
    ScopedFramebuffer shadowMap(*this->shadowMapFBO, GL_FRAMEBUFFER);

    this->shadowMapFBO->attachTexture2D(GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->shadowMapTexture->getId(), 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    this->shadowMapFBO->checkComplete();
  }

  glBindTexture(GL_TEXTURE_2D, 0);
};

// Constructor
DirectionalShadow::DirectionalShadow(const GLuint width, const GLuint height) : Shadow(width, height)
{
  this->shadowMapTexture = std::make_unique<Texture>(GL_TEXTURE_2D);

  this->init();
};

// Public functions
void DirectionalShadow::bind(Shader &shader, int textureUnit, const std::string &name) const
{
  const std::string &finalName = name.empty() ? "directionalShadowMap" : name;

  this->shadowMapTexture->activate(textureUnit);
  this->shadowMapTexture->bind();
  // change
  shader.set1i(textureUnit, finalName);
}