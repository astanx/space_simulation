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
void Mesh::initVAO()
{
  if (!this->VAO)
    this->VAO = std::make_unique<VertexArray>();

  ScopedVertexArray vao(*this->VAO);

  bool wasInitVBO = bool(this->VBO);
  if (!wasInitVBO)
    this->VBO = std::make_unique<Buffer>();

  this->VBO->bind(GL_ARRAY_BUFFER);

  GLint currentVBOSize = 0;
  glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &currentVBOSize);
  if (!wasInitVBO || currentVBOSize != this->vertices.size() * sizeof(Vertex))
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), this->vertices.data(), GL_STATIC_DRAW));
  else
  {
    void *ptr = glMapBufferRange(GL_ARRAY_BUFFER, 0,
                                 this->vertices.size() * sizeof(Vertex),
                                 GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    if (ptr)
    {
      memcpy(ptr, this->vertices.data(), this->vertices.size() * sizeof(Vertex));
      glUnmapBuffer(GL_ARRAY_BUFFER);
    }
  }

  if (this->indices.size() > 0)
  {
    bool wasInitEBO = bool(this->EBO);
    if (!wasInitEBO)
      this->EBO = std::make_unique<Buffer>();

    this->EBO->bind(GL_ELEMENT_ARRAY_BUFFER);

    GLint currentEBOSize = 0;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &currentEBOSize);
    if (!wasInitEBO || currentEBOSize != this->indices.size() * sizeof(GLuint))
      GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), this->indices.data(), GL_STATIC_DRAW));
    else
    {
      void *ptr = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0,
                                   this->indices.size() * sizeof(GLuint),
                                   GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
      if (ptr)
      {
        memcpy(ptr, this->indices.data(), this->indices.size() * sizeof(GLuint));
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
            sizeof(Vertex),
            (void *)attr.offset);
      else
        glVertexAttribPointer(
            attr.index,
            attr.size,
            attr.type,
            attr.normalized,
            sizeof(Vertex),
            (void *)attr.offset);
      glEnableVertexAttribArray(attr.index);
    }

    this->VAO->setLayoutInitialized(true);
  }

  // // Position attribute
  // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, position));
  // glEnableVertexAttribArray(0);

  // if (this->layout != VertexLayout::PositionOnly)
  // {
  //   // Texcoord attribute
  //   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, texcoord));
  //   glEnableVertexAttribArray(1);

  //   // Normal attribute
  //   glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, normal));
  //   glEnableVertexAttribArray(2);
  // }
  // if (this->layout == VertexLayout::Full)
  // {
  //   // Color attribute
  //   glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, color));
  //   glEnableVertexAttribArray(3);
  // }
}

// Constructor and Destructor
Mesh::Mesh(std::vector<Vertex> *vertexArray, std::vector<GLuint> *indexArray, VertexLayout layout, GLenum drawMode)
{
  if (vertexArray)
    this->vertices = *vertexArray;
  else
    this->vertices = std::vector<Vertex>();

  if (indexArray)
    this->indices = *indexArray;
  else
    this->indices = std::vector<GLuint>();

  this->drawMode = drawMode;

  auto it = LAYOUTS.find(layout);
  if (it == LAYOUTS.end())
    throw std::runtime_error("[Mesh] RUNTIME ERROR: Invalid vertex layout");
  this->VAOlayout = it->second;

  this->initVAO();
}

Mesh::Mesh(std::unique_ptr<Primitive> primitive, VertexLayout layout, GLenum drawMode)
{
  this->vertices = primitive->getVertices();
  this->indices = primitive->getIndices();

  this->drawMode = drawMode;

  auto it = LAYOUTS.find(layout);
  if (it == LAYOUTS.end())
    throw std::runtime_error("[Mesh] RUNTIME ERROR: Invalid vertex layout");
  this->VAOlayout = it->second;

  this->initVAO();
}

Mesh::Mesh(const Mesh &obj)
{
  this->vertices = obj.vertices;
  this->indices = obj.indices;

  this->drawMode = obj.drawMode;
  this->VAOlayout = obj.VAOlayout;

  this->initVAO();
}

Mesh::~Mesh()
{
}

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

  if (this->indices.size() == 0)
    GL_CALL(glDrawArrays(this->drawMode, 0, this->vertices.size()));
  else
    GL_CALL(glDrawElements(this->drawMode, this->indices.size(), GL_UNSIGNED_INT, 0));
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

  if (this->indices.size() == 0)
    GL_CALL(glDrawArraysInstanced(this->drawMode, 0, this->vertices.size(), this->instanceCount));
  else
    GL_CALL(glDrawElementsInstanced(
        this->drawMode,
        this->indices.size(),
        GL_UNSIGNED_INT,
        0,
        this->instanceCount));
}

const double Mesh::calculateVolume() const
{
  double volume = 0.0;

  for (size_t i = 0; i < this->indices.size(); i += 3)
  {
    const glm::vec3 &v0 = vertices[indices[i]].position;
    const glm::vec3 &v1 = vertices[indices[i + 1]].position;
    const glm::vec3 &v2 = vertices[indices[i + 2]].position;

    volume += glm::dot(v0, glm::cross(v1, v2));
  }

  return std::abs(volume) / 6.0;
}

void Mesh::updateBuffers(std::vector<Vertex> *vertexArray, std::vector<GLuint> *indexArray)
{
  if (vertexArray)
    this->vertices = *vertexArray;
  else
    this->vertices = std::vector<Vertex>();

  if (indexArray)
    this->indices = *indexArray;
  else
    this->indices = std::vector<GLuint>();

  this->initVAO();
}