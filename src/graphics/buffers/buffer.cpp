#include "graphics/buffers/buffer.h"

// Constructor and Destructor
Buffer::Buffer()
{
  glGenBuffers(1, &this->id);
}

Buffer::~Buffer()
{
  glDeleteBuffers(1, &this->id);
}

// Public functions
void Buffer::bind(GLenum target) const
{
  glBindBuffer(target, this->id);
}

void Buffer::unbind(GLenum target) const
{
  glBindBuffer(target, 0);
}

void Buffer::bindBufferBase(GLenum target, GLint binding) const
{
  glBindBufferBase(target, binding, this->id);
}