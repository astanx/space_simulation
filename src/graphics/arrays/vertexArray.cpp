#include "graphics/arrays/vertexArray.h"

// Constructor and Destructor
VertexArray::VertexArray()
{
  glGenVertexArrays(1, &this->id);
}

VertexArray::~VertexArray()
{
  glDeleteVertexArrays(1, &this->id);
}

// Public functions
void VertexArray::bind() const
{
  glBindVertexArray(this->id);
}

void VertexArray::unbind() const
{
  glBindVertexArray(0);
}