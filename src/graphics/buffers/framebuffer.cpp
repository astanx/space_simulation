#include "graphics/buffers/framebuffer.h"

#include "debug/logger.h"

// Constructor and Destructor
Framebuffer::Framebuffer() : Buffer()
{
  glGenFramebuffers(1, &this->id);
}

Framebuffer::~Framebuffer()
{
  glDeleteFramebuffers(1, &this->id);
}

// Public functions
void Framebuffer::bind(GLenum target) const
{
  glBindFramebuffer(target, this->id);
}

void Framebuffer::unbind(GLenum target) const
{
  glBindFramebuffer(target, 0);
}

bool Framebuffer::checkComplete() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, this->id);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    Logger::logError("Framebuffer", "Framebuffer is not complete");
    return false;
  }

  return true;
}

void Framebuffer::attachTexture(GLenum attachment, GLuint texture, GLint level)
{
  GL_CALL(glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture, level));
}
void Framebuffer::attachTexture2D(GLenum attachment, GLenum target, GLuint texture, GLint level)
{
  GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, target, texture, level));
}
void Framebuffer::attachRenderBuffer(GLenum attachment, GLenum target, GLuint buffer)
{
  GL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, target, buffer));
}
