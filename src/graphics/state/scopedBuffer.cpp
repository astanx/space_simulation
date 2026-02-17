#include "graphics/state/scopedBuffer.h"

#include "debug/logger.h"

GLenum ScopedBuffer::getBindingEnumForTarget(GLenum target)
{
  switch (target)
  {
  case GL_ARRAY_BUFFER:
    return GL_ARRAY_BUFFER_BINDING;
  case GL_ELEMENT_ARRAY_BUFFER:
    return GL_ELEMENT_ARRAY_BUFFER_BINDING;
  case GL_UNIFORM_BUFFER:
    return GL_UNIFORM_BUFFER_BINDING;
  default:
    Logger::logError("Scoped buffer", "Can not find binding enum for target - " + std::to_string(target));
    return GL_ARRAY_BUFFER_BINDING;
  }
}

ScopedBuffer::ScopedBuffer(const Buffer &buffer, GLenum target, bool saveState)
{
  this->target = target;

  if (saveState)
    glGetIntegerv(this->getBindingEnumForTarget(target), &this->prevBuffer);

  buffer.bind(target);
};

ScopedBuffer::ScopedBuffer(const GLint &buffer, GLenum target, bool saveState)
{
  this->target = target;

  if (saveState)
    glGetIntegerv(this->getBindingEnumForTarget(target), &this->prevBuffer);

  glBindBuffer(target, buffer);
};

ScopedBuffer::~ScopedBuffer()
{
  glBindBuffer(this->target, this->prevBuffer);
}