#include "scene/shadow/shadow.h"

// Constructor / Destructor
Shadow::Shadow(GLuint width, GLuint height)
{
  this->shadowWidth = width;
  this->shadowHeight = height;

  this->shadowMapFBO = std::make_unique<Framebuffer>();
}

Shadow::~Shadow()
{
  if (glIsTexture(this->shadowMapTexture))
    glDeleteTextures(1, &this->shadowMapTexture);
}
