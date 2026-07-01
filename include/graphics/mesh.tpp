#pragma once

#include "debug/logger.h"

#include "graphics/instanceLayouts.h"

#include "graphics/primitives/primitives.h"

#include "graphics/state/scopedBuffer.h"
#include "graphics/state/scopedVertexArray.h"

// Private functions
template <typename T>
void Mesh::initVAO(std::vector<T> *vertexArray, std::vector<GLuint> *indexArray)
{
  if (!this->VAO)
    this->VAO = std::make_unique<VertexArray>();

  this->verticesSize = vertexArray ? vertexArray->size() : 0;
  this->indicesSize = indexArray ? indexArray->size() : 0;

  ScopedVertexArray vao(*this->VAO);

  bool wasInitVBO = bool(this->VBO);
  if (!wasInitVBO)
    this->VBO = std::make_unique<Buffer>();

  this->VBO->bind(GL_ARRAY_BUFFER);

  GLint currentVBOSize = 0;
  glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &currentVBOSize);
  if (!wasInitVBO || currentVBOSize != this->verticesSize * sizeof(T))
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, this->verticesSize * sizeof(T), vertexArray->data(), GL_STATIC_DRAW));
  else
  {
    void *ptr = glMapBufferRange(GL_ARRAY_BUFFER, 0,
                                 this->verticesSize * sizeof(T),
                                 GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    if (ptr)
    {
      memcpy(ptr, vertexArray->data(), this->verticesSize * sizeof(T));
      glUnmapBuffer(GL_ARRAY_BUFFER);
    }
  }

  if (this->indicesSize > 0)
  {
    bool wasInitEBO = bool(this->EBO);
    if (!wasInitEBO)
      this->EBO = std::make_unique<Buffer>();

    this->EBO->bind(GL_ELEMENT_ARRAY_BUFFER);

    GLint currentEBOSize = 0;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &currentEBOSize);
    if (!wasInitEBO || currentEBOSize != this->indicesSize * sizeof(GLuint))
      GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indicesSize * sizeof(GLuint), indexArray->data(), GL_STATIC_DRAW));
    else
    {
      void *ptr = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0,
                                   this->indicesSize * sizeof(GLuint),
                                   GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
      if (ptr)
      {
        memcpy(ptr, indexArray->data(), this->indicesSize * sizeof(GLuint));
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
      }
    }
  }

  if (!this->VAO->isLayoutInitialized())
  {
    for (size_t i = 0; i < this->VAOlayout.count; ++i)
    {
      const VertexAttribute &attr = this->VAOlayout.attributes[i];
      if (attr.type == GL_UNSIGNED_INT || attr.type == GL_INT)
        glVertexAttribIPointer(
            attr.index,
            attr.size,
            attr.type,
            sizeof(T),
            (void *)attr.offset);
      else
        glVertexAttribPointer(
            attr.index,
            attr.size,
            attr.type,
            attr.normalized,
            sizeof(T),
            (void *)attr.offset);
      glEnableVertexAttribArray(attr.index);
    }

    this->VAO->setLayoutInitialized(true);
  }
}

// Constructor
template <typename T>
Mesh::Mesh(std::vector<T> *vertexArray, std::vector<GLuint> *indexArray, VertexLayout layout, GLenum drawMode)
{
  this->drawMode = drawMode;

  auto it = LAYOUTS.find(layout);
  if (it == LAYOUTS.end())
    throw std::runtime_error("[Mesh] RUNTIME ERROR: Invalid vertex layout");
  this->VAOlayout = it->second;

  this->initVAO(vertexArray, indexArray);
}

template <typename T>
Mesh::Mesh(TypeTag<T>, std::unique_ptr<Primitive> primitive, VertexLayout layout, GLenum drawMode)
{
  this->drawMode = drawMode;

  auto it = LAYOUTS.find(layout);
  if (it == LAYOUTS.end())
    throw std::runtime_error("[Mesh] RUNTIME ERROR: Invalid vertex layout");
  this->VAOlayout = it->second;

  std::vector<T> vertices = primitive->getVertices<T>();
  std::vector<GLuint> indices = primitive->getIndices();

  this->initVAO(&vertices, &indices);
}

// Public functions
template <typename T>
void Mesh::updateBuffers(std::vector<T> *vertexArray, std::vector<GLuint> *indexArray)
{
  if (vertexArray)
    this->verticesSize = vertexArray->size();
  else
    this->verticesSize = 0;

  if (indexArray)
    this->indicesSize = indexArray->size();
  else
    this->indicesSize = 0;

  this->initVAO(vertexArray, indexArray);
}

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