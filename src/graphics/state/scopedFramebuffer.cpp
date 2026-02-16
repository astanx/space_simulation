#include "graphics/state/scopedFramebuffer.h"

#include "debug/logger.h"

GLenum ScopedFramebuffer::getBindingEnumForTarget(GLenum target)
{
  switch (target)
  {
  case GL_FRAMEBUFFER:
    return GL_DRAW_FRAMEBUFFER;

  case GL_DRAW_FRAMEBUFFER:
    return GL_DRAW_FRAMEBUFFER;

  case GL_READ_FRAMEBUFFER:
    return GL_READ_FRAMEBUFFER;

  default:
    Logger::logError("Scoped framebuffer", "Can not find binding enum for target - " + std::to_string(target));
    return GL_DRAW_FRAMEBUFFER;
  }
}

ScopedFramebuffer::ScopedFramebuffer(const Framebuffer &buffer, GLenum target, bool saveState)
{
  if (saveState)
  {

    this->binding = this->getBindingEnumForTarget(target);
    glGetIntegerv(binding, &this->prevFBO);
  }

  buffer.bind(target);
};

ScopedFramebuffer::ScopedFramebuffer(const GLint &buffer, GLenum target, bool saveState)
{
  if (saveState)
  {
    this->binding = this->getBindingEnumForTarget(target);
    glGetIntegerv(binding, &this->prevFBO);
  }

  glBindFramebuffer(target, buffer);
};

ScopedFramebuffer::~ScopedFramebuffer()
{
  glBindFramebuffer(binding, this->prevFBO);
}