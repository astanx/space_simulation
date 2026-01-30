#include "scene/shadow/directionalShadow.h"
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
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glBindTexture(GL_TEXTURE_2D, 0);
};

// Constructor
DirectionalShadow::DirectionalShadow(const GLuint width, const GLuint height) : Shadow(width, height)
{
  this->init();
};

// Public functions
void DirectionalShadow::bindShadowMapFBO() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, this->shadowMapFBO);
  // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->shadowMapTexture, 0);
  // glDrawBuffer(GL_NONE);
  // glReadBuffer(GL_NONE);

  // if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  // {
  //   std::cerr << "ERROR::SHADOW::FRAMEBUFFER_NOT_COMPLETE" << std::endl;
  // }
}

void DirectionalShadow::bind(Shader &shader) const
{
  glBindTexture(GL_TEXTURE_2D, this->shadowMapTexture);

  // change
  shader.set1i(0, "directionalShadowMap");
}