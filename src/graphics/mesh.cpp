#include "graphics/mesh.h"

#include "debug/logger.h"

#include "graphics/shader.h"
#include "graphics/vertex.h"
#include "graphics/instanceLayouts.h"

#include "graphics/state/scopedBuffer.h"
#include "graphics/state/scopedVertexArray.h"

#include "graphics/primitives/primitives.h"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

// Private functions
void Mesh::bindInstanceAttributes(const Buffer &vbo, size_t size, size_t offset) const
{
  GLuint start = this->instanceLayout.offset ? this->VAOlayout.count : 0;

  ScopedBuffer buff(vbo, GL_ARRAY_BUFFER);

  for (size_t i = 0; i < this->instanceLayout.count; i++)
  {
    const VertexAttribute &attr = this->instanceLayout.attributes[i];
    if (attr.type == GL_UNSIGNED_INT || attr.type == GL_INT)
      glVertexAttribIPointer(
          start + attr.index,
          attr.size,
          attr.type,
          size,
          (void *)(attr.offset + offset * size));
    else
      glVertexAttribPointer(
          start + attr.index,
          attr.size,
          attr.type,
          attr.normalized,
          size,
          (void *)(attr.offset + offset * size));

    glEnableVertexAttribArray(start + attr.index);
    glVertexAttribDivisor(start + attr.index, 1);
  }
}

// Functions
void Mesh::setInstanceLayout(InstanceLayout layout)
{
  auto it = INSTANCE_LAYOUTS.find(layout);
  if (it == INSTANCE_LAYOUTS.end())
    Logger::logFatal("Mesh", "Invalid instance layout");
  this->instanceLayout = it->second;
}

void Mesh::render() const
{
  std::optional<ScopedVertexArray> vaoScope;

  if (this->VAO)
    vaoScope.emplace(*this->VAO);
  else
    Logger::logError("Mesh", "No VAO to render");

  if (this->indicesSize == 0)
    GL_CALL(glDrawArrays(this->drawMode, 0, this->verticesSize));
  else
    GL_CALL(glDrawElements(this->drawMode, this->indicesSize, GL_UNSIGNED_INT, 0));
};

void Mesh::renderInstanced(Buffer *instanceVBO, size_t size, size_t count, size_t offset) const
{
  if ((!this->instancingInitialized || this->instanceCount == 0) && (!instanceVBO || count == 0))
    return;

  std::optional<ScopedVertexArray> vaoScope;

  if (this->VAO)
    vaoScope.emplace(*this->VAO);
  else
    Logger::logError("Mesh", "No instanced VAO to render");

  if (instanceVBO)
    this->bindInstanceAttributes(*instanceVBO, size, offset);

  unsigned int drawCount = instanceVBO ? count : this->instanceCount;

  if (this->indicesSize == 0)
    GL_CALL(glDrawArraysInstanced(this->drawMode, 0, this->verticesSize, drawCount));
  else
    GL_CALL(glDrawElementsInstanced(
        this->drawMode,
        this->indicesSize,
        GL_UNSIGNED_INT,
        0,
        drawCount));
}