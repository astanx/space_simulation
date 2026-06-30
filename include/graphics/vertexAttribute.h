#pragma once

#include <GL/glew.h>

struct VertexAttribute
{
  GLuint index;
  GLint size;
  GLenum type;
  GLboolean normalized;
  size_t offset;
};

struct LayoutDesc
{
  const VertexAttribute *attributes;
  size_t count;
};