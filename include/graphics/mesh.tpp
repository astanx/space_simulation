#pragma once

#include "debug/logger.h"

#include "graphics/instanceLayouts.h"

#include "graphics/state/scopedBuffer.h"
#include "graphics/state/scopedVertexArray.h"

template <typename T>
void Mesh::bindInstanceAttributes(const T *instanceData, const Buffer &vbo) const
{
  GLuint start = this->VAOlayout.count;

  ScopedBuffer buff(vbo, GL_ARRAY_BUFFER);

  for (size_t i = 0; i < this->instanceLayout.count; i++)
  {
    const VertexAttribute &attr = this->instanceLayout.attributes[i];
    if (attr.type == GL_UNSIGNED_INT || attr.type == GL_INT)
      glVertexAttribIPointer(
          start + attr.index,
          attr.size,
          attr.type,
          sizeof(T),
          (void *)attr.offset);
    else
      glVertexAttribPointer(
          start + attr.index,
          attr.size,
          attr.type,
          attr.normalized,
          sizeof(T),
          (void *)attr.offset);

    glEnableVertexAttribArray(start + attr.index);
    glVertexAttribDivisor(start + attr.index, 1);
  }
}

template <typename T>
void Mesh::setInstanceBuffer(const T *instanceData, size_t count, size_t vboCount)
{
  if (vboCount == 0)
  {
    vboCount = 1;
    Logger::logWarning("Mesh", "VBO count cannot be 0, defaulting to 1");
  }
  if (!this->instancingInitialized)
  {
    for (size_t i = 0; i < vboCount; i++)
      this->instanceVBOS.push_back(std::make_unique<Buffer>());

    this->instancingInitialized = true;
  }

  this->instanceCount = static_cast<unsigned int>(count);

  std::optional<ScopedVertexArray> vaoScope;

  if (this->VAO)
    vaoScope.emplace(*VAO);
  else
    Logger::logError("Mesh", "No instanced VAO to set buffer");

  for (std::unique_ptr<Buffer> &vbo : this->instanceVBOS)
  {
    ScopedBuffer buff(*vbo, GL_ARRAY_BUFFER);

    GL_CALL(glBufferData(
        GL_ARRAY_BUFFER,
        this->instanceCount * sizeof(T),
        instanceData,
        GL_DYNAMIC_DRAW));
  }

  bindInstanceAttributes(instanceData, *this->instanceVBOS[this->vboIndex]);
}

template <typename T>
void Mesh::updateInstanceBuffer(const T *instanceData, size_t count, size_t vboCount)
{
  if (!instancingInitialized || this->instanceVBOS.empty() || vboCount != this->instanceVBOS.size())
  {
    setInstanceBuffer(instanceData, count, vboCount);
    Logger::logWarning("Mesh", "Update buffer called before buffer is set, setting buffer");
    return;
  }

  this->instanceCount = static_cast<unsigned int>(count);

  this->vboIndex = (this->vboIndex + 1) % this->instanceVBOS.size();

  Buffer &vbo = *this->instanceVBOS[this->vboIndex];

  {
    std::optional<ScopedVertexArray> vaoScope;
    if (this->VAO)
      vaoScope.emplace(*this->VAO);
    else
      Logger::logError("Mesh", "No instanced VAO to set buffer");

    this->bindInstanceAttributes(instanceData, vbo);
  }

  ScopedBuffer buff(vbo, GL_ARRAY_BUFFER);

  glBufferData(GL_ARRAY_BUFFER, instanceCount * sizeof(T), nullptr, GL_DYNAMIC_DRAW);

  void *ptr = glMapBufferRange(
      GL_ARRAY_BUFFER,
      0,
      instanceCount * sizeof(T),
      GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

  if (!ptr)
    return;

  memcpy(ptr, instanceData, instanceCount * sizeof(T));

  GL_CALL(glUnmapBuffer(GL_ARRAY_BUFFER));
}