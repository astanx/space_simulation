#include "scene/shadow/shadow.h"

// Constructor / Destructor
Shadow::Shadow(GLuint width, GLuint height)
{
  this->shadowWidth = width;
  this->shadowHeight = height;

  this->shadowMapFBO = std::make_unique<Framebuffer>();
}
