#include "graphics/mesh.h"

#include "debug/logger.h"

#include "graphics/shader.h"
#include "graphics/vertex.h"

#include "graphics/state/scopedBuffer.h"
#include "graphics/state/scopedVertexArray.h"

#include "graphics/primitives/primitives.h"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

// Private functions
void Mesh::initVAO()
{
  if (!this->VAOS[this->layout])
    this->VAOS[this->layout] = std::make_unique<VertexArray>();

  ScopedVertexArray vao(*this->VAOS[this->layout]);

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

  if (!this->VAOS[this->layout]->isLayoutInitialized())
  {
    auto it = LAYOUTS.find(this->layout);
    if (it == LAYOUTS.end())
      throw std::runtime_error("[Mesh] RUNTIME ERROR: Invalid vertex layout");
    const LayoutDesc &layout = it->second;

    for (size_t i = 0; i < layout.count; ++i)
    {
      const VertexAttribute &attr = layout.attributes[i];
      glVertexAttribPointer(
          attr.index,
          attr.size,
          attr.type,
          attr.normalized,
          sizeof(Vertex),
          (void *)attr.offset);
      glEnableVertexAttribArray(attr.index);
    }

    this->VAOS[this->layout]->setLayoutInitialized(true);
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
  this->layout = layout;

  this->initVAO();
}

Mesh::Mesh(std::unique_ptr<Primitive> primitive, VertexLayout layout, GLenum drawMode)
{
  this->vertices = primitive->getVertices();
  this->indices = primitive->getIndices();

  this->drawMode = drawMode;
  this->layout = layout;

  this->initVAO();
}

Mesh::Mesh(const Mesh &obj)
{
  this->vertices = obj.vertices;
  this->indices = obj.indices;

  this->drawMode = obj.drawMode;
  this->layout = obj.layout;

  this->initVAO();
}

Mesh::~Mesh()
{
}

// Functions
void Mesh::setInstanceBuffer(InstanceData* instanceData, size_t count)
{
  if (!instancingInitialized)
  {
    this->instanceVBO = std::make_unique<Buffer>();
    instancingInitialized = true;
  }

  this->instanceCount = static_cast<unsigned int>(count);

  const std::unique_ptr<VertexArray> &vao = this->VAOS.at(this->layout);

  std::optional<ScopedVertexArray> vaoScope;

  if (vao)
    vaoScope.emplace(*vao);
  else
    Logger::logError("Mesh", "No instanced VAO to set buffer");

  this->instanceVBO->bind(GL_ARRAY_BUFFER);

  GL_CALL(glBufferData(
      GL_ARRAY_BUFFER,
      this->instanceCount * sizeof(InstanceData),
      instanceData,
      GL_DYNAMIC_DRAW));

  auto it = LAYOUTS.find(this->layout);
  if (it == LAYOUTS.end())
    throw std::runtime_error("[Mesh] RUNTIME ERROR: Invalid vertex layout");
  const LayoutDesc &layout = it->second;

  GLuint start = layout.count;

  // for (int i = 0; i < 4; i++)
  // {
  //   glEnableVertexAttribArray(INSTANCE_ATTRIB_START + i);
  //   glVertexAttribPointer(
  //       INSTANCE_ATTRIB_START + i,
  //       4,
  //       GL_FLOAT,
  //       GL_FALSE,
  //       sizeof(InstanceData),
  //       (void *)(sizeof(glm::vec4) * i));
  //   glVertexAttribDivisor(INSTANCE_ATTRIB_START + i, 1);
  // }

  glEnableVertexAttribArray(start);
  glVertexAttribPointer(
      start,
      3,
      GL_FLOAT,
      GL_FALSE,
      sizeof(InstanceData), 0);
  glVertexAttribDivisor(start, 1);
}

void Mesh::updateInstanceBuffer(InstanceData* instanceData, size_t count)
{
  if (!instancingInitialized)
  {
    setInstanceBuffer(instanceData, count);
    Logger::logWarning("Mesh", "Update buffer called before buffer is set, setting buffer");
    return;
  }

  this->instanceCount = static_cast<unsigned int>(count);

  ScopedBuffer vbo(*this->instanceVBO, GL_ARRAY_BUFFER);

  void *ptr = glMapBufferRange(
      GL_ARRAY_BUFFER,
      0,
      instanceCount * sizeof(InstanceData),
      GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

  if (!ptr)
    return;

  memcpy(ptr, instanceData, instanceCount * sizeof(InstanceData));

  GL_CALL(glUnmapBuffer(GL_ARRAY_BUFFER));
}

void Mesh::render() const
{
  const std::unique_ptr<VertexArray> &vao = this->VAOS.at(this->layout);

  std::optional<ScopedVertexArray> vaoScope;

  if (vao)
    vaoScope.emplace(*vao);
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

  const std::unique_ptr<VertexArray> &vao = this->VAOS.at(this->layout);

  std::optional<ScopedVertexArray> vaoScope;

  if (vao)
    vaoScope.emplace(*vao);
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