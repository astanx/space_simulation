#include "scene/shadow/shadow.h"

// Constructor / Destructor
Shadow::Shadow(GLuint width, GLuint height)
{
  this->shadowWidth = width;
  this->shadowHeight = height;
}

Shadow::~Shadow()
{
  if (glIsFramebuffer(this->shadowMapFBO))
    glDeleteFramebuffers(1, &this->shadowMapFBO);
  if (glIsTexture(this->shadowMapTexture))
    glDeleteTextures(1, &this->shadowMapTexture);
}

// Public functions
void Shadow::unbindShadowMapFBO() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
