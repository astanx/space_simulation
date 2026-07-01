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

// Functions
void Mesh::setInstanceLayout(InstanceLayout layout)
{
  auto it = INSTANCE_LAYOUTS.find(layout);
  if (it == INSTANCE_LAYOUTS.end())
    throw std::runtime_error("[Mesh] RUNTIME ERROR: Invalid instance layout");
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

void Mesh::renderInstanced() const
{
  if (!this->instancingInitialized || this->instanceCount == 0)
    return;

  std::optional<ScopedVertexArray> vaoScope;

  if (this->VAO)
    vaoScope.emplace(*this->VAO);
  else
    Logger::logError("Mesh", "No instanced VAO to render");

  if (this->indicesSize == 0)
    GL_CALL(glDrawArraysInstanced(this->drawMode, 0, this->verticesSize, this->instanceCount));
  else
    GL_CALL(glDrawElementsInstanced(
        this->drawMode,
        this->indicesSize,
        GL_UNSIGNED_INT,
        0,
        this->instanceCount));
}