#include "scene/shadow/shadow.h"

// Constructor / Destructor
Shadow::Shadow(GLuint width, GLuint height)
{
  this->shadowWidth = width;
  this->shadowHeight = height;
}

Shadow::~Shadow()
{
  glDeleteFramebuffers(1, &this->shadowMapFBO);
  glDeleteTextures(1, &this->shadowMapTexture);
}

// Public functions
void Shadow::unbindShadowMapFBO() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
