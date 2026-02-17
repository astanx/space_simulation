#include "graphics/state/scopedVertexArray.h"

ScopedVertexArray::ScopedVertexArray(const VertexArray &array, bool saveState)
{
  if (saveState)
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &this->prevArray);

  array.bind();
};

ScopedVertexArray::ScopedVertexArray(const GLint &array, bool saveState)
{
  if (saveState)
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &this->prevArray);

  glBindVertexArray(array);
};

ScopedVertexArray::~ScopedVertexArray()
{
  glBindVertexArray(this->prevArray);
}