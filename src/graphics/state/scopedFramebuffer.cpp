#include "graphics/state/scopedFramebuffer.h"

#include "debug/logger.h"

GLenum ScopedFramebuffer::getBindingEnumForTarget(GLenum target)
{
  switch (target)
  {
  case GL_FRAMEBUFFER:
    return GL_DRAW_FRAMEBUFFER_BINDING;

  case GL_DRAW_FRAMEBUFFER:
    return GL_DRAW_FRAMEBUFFER_BINDING;

  case GL_READ_FRAMEBUFFER:
    return GL_READ_FRAMEBUFFER_BINDING;

  default:
    Logger::logError("Scoped framebuffer", "Can not find binding enum for target - " + std::to_string(target));
    return GL_DRAW_FRAMEBUFFER_BINDING;
  }
}

ScopedFramebuffer::ScopedFramebuffer(const Framebuffer &buffer, GLenum target, bool saveState)
{
  this->target = target;

  if (saveState)
    glGetIntegerv(this->getBindingEnumForTarget(target), &this->prevFBO);

  buffer.bind(target);
};

ScopedFramebuffer::ScopedFramebuffer(const GLint &buffer, GLenum target, bool saveState)
{
  this->target = target;

  if (saveState)
    glGetIntegerv(this->getBindingEnumForTarget(target), &this->prevFBO);

  glBindFramebuffer(target, buffer);
};

ScopedFramebuffer::~ScopedFramebuffer()
{
  glBindFramebuffer(this->target, this->prevFBO);
}