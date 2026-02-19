#include "graphics/buffers/renderBuffer.h"

// Constructor and Destructor
RenderBuffer::RenderBuffer()
{
  glGenRenderbuffers(1, &this->id);
}

RenderBuffer::~RenderBuffer()
{
  glDeleteRenderbuffers(1, &this->id);
}

// Public functions
void RenderBuffer::bind(GLenum target) const
{
  glBindRenderbuffer(target, this->id);
}

void RenderBuffer::unbind(GLenum target) const
{
  glBindRenderbuffer(target, 0);
}