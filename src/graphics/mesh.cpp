#include "graphics/mesh.h"

#include "debug/logger.h"

#include "graphics/shader.h"
#include "graphics/vertex.h"

#include "graphics/primitives/primitives.h"

#include <glm/gtc/matrix_transform.hpp>

// Private functions
void Mesh::initVAO()
{
  if (!glIsVertexArray(this->VAOS[this->layout]))
    glGenVertexArrays(1, &this->VAOS[this->layout]);

  glBindVertexArray(this->VAOS[this->layout]);

  if (!glIsBuffer(this->VBO))
    glGenBuffers(1, &this->VBO);

  glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
  glBufferData(GL_ARRAY_BUFFER, this->nrOfVertices * sizeof(Vertex), this->vertices, GL_STATIC_DRAW);

  if (this->nrOfIndices > 0)
  {
    if (!glIsBuffer(this->EBO))
      glGenBuffers(1, &this->EBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->nrOfIndices * sizeof(GLuint), this->indices, GL_STATIC_DRAW);
  }

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

  glBindVertexArray(0);
}

// Constructor and Destructor
Mesh::Mesh(Vertex *vertexArray, const unsigned &nrOfVertices, GLuint *indexArray, const unsigned &nrOfIndices, VertexLayout layout, GLenum drawMode)
{
  this->nrOfVertices = nrOfVertices;
  this->nrOfIndices = nrOfIndices;

  this->vertices = new Vertex[this->nrOfVertices];
  for (size_t i = 0; i < this->nrOfVertices; i++)
  {
    this->vertices[i] = vertexArray[i];
  }

  this->indices = new GLuint[this->nrOfIndices];
  for (size_t i = 0; i < this->nrOfIndices; i++)
  {
    this->indices[i] = indexArray[i];
  }

  this->drawMode = drawMode;
  this->layout = layout;

  this->initVAO();
}

Mesh::Mesh(std::unique_ptr<Primitive> primitive, VertexLayout layout, GLenum drawMode)
{
  this->nrOfVertices = primitive->getNrOfVertices();
  this->nrOfIndices = primitive->getNrOfIndices();

  Vertex *vertexArray = primitive->getVertices();
  this->vertices = new Vertex[this->nrOfVertices];
  for (size_t i = 0; i < this->nrOfVertices; i++)
  {
    this->vertices[i] = vertexArray[i];
  }

  GLuint *indexArray = primitive->getIndices();
  this->indices = new GLuint[this->nrOfIndices];
  for (size_t i = 0; i < this->nrOfIndices; i++)
  {
    this->indices[i] = indexArray[i];
  }

  this->drawMode = drawMode;
  this->layout = layout;

  this->initVAO();
}

Mesh::Mesh(const Mesh &obj)
{
  this->nrOfVertices = obj.nrOfVertices;
  this->nrOfIndices = obj.nrOfIndices;

  this->vertices = new Vertex[this->nrOfVertices];
  for (size_t i = 0; i < this->nrOfVertices; i++)
  {
    this->vertices[i] = obj.vertices[i];
  }

  this->indices = new GLuint[this->nrOfIndices];
  for (size_t i = 0; i < this->nrOfIndices; i++)
  {
    this->indices[i] = obj.indices[i];
  }

  this->drawMode = obj.drawMode;
  this->layout = obj.layout;

  this->initVAO();
}

Mesh::~Mesh()
{
  if (glIsVertexArray(this->VAOS[this->layout]))
    glDeleteVertexArrays(1, &this->VAOS[this->layout]);

  if (glIsBuffer(this->VBO))
    glDeleteBuffers(1, &this->VBO);

  if (glIsBuffer(this->EBO))
    glDeleteBuffers(1, &this->EBO);

  if (glIsBuffer(this->instanceVBO))
    glDeleteBuffers(1, &instanceVBO);

  delete[] this->vertices;
  delete[] this->indices;
}

// Functions
void Mesh::setInstanceBuffer(const std::vector<InstanceData> &instanceData)
{
  if (!instancingInitialized)
  {
    glGenBuffers(1, &this->instanceVBO);
    instancingInitialized = true;
  }

  this->instanceCount = static_cast<unsigned int>(instanceData.size());

  GLuint vao = this->VAOS.at(this->layout);

  if (glIsVertexArray(vao))
    glBindVertexArray(vao);
  else
    Logger::logError("Mesh", "No instanced VAO to set buffer");

  glBindBuffer(GL_ARRAY_BUFFER, this->instanceVBO);

  glBufferData(
      GL_ARRAY_BUFFER,
      this->instanceCount * sizeof(InstanceData),
      instanceData.data(),
      GL_DYNAMIC_DRAW);

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

  glBindVertexArray(0);
}

void Mesh::updateInstanceBuffer(const std::vector<InstanceData> &instanceData)
{
  if (!instancingInitialized)
  {
    setInstanceBuffer(instanceData);
    Logger::logWarning("Mesh", "Update buffer called before buffer is set, setting buffer");
    return;
  }

  if (glIsBuffer(this->instanceVBO))
    glBindBuffer(GL_ARRAY_BUFFER, this->instanceVBO);
  else
    Logger::logError("Mesh", "No instance VBO to update buffer");

  void *ptr = glMapBufferRange(
      GL_ARRAY_BUFFER,
      0,
      instanceCount * sizeof(InstanceData),
      GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

  if (!ptr)
    return; // mapping failed

  memcpy(ptr, instanceData.data(), instanceCount * sizeof(InstanceData));

  glUnmapBuffer(GL_ARRAY_BUFFER);
}

void Mesh::render() const
{
  GLuint vao = this->VAOS.at(this->layout);

  if (glIsVertexArray(vao))
    glBindVertexArray(vao);
  else
    Logger::logError("Mesh", "No VAO to render");

  if (this->nrOfIndices == 0)
    glDrawArrays(this->drawMode, 0, this->nrOfVertices);
  else
    glDrawElements(this->drawMode, this->nrOfIndices, GL_UNSIGNED_INT, 0);
};

void Mesh::renderInstanced() const
{
  if (!this->instancingInitialized || this->instanceCount == 0)
    return;

  GLuint vao = this->VAOS.at(this->layout);

  if (glIsVertexArray(vao))
    glBindVertexArray(vao);
  else
    Logger::logError("Mesh", "No instanced VAO to render");

  if (this->nrOfIndices == 0)
    glDrawArraysInstanced(this->drawMode, 0, this->nrOfVertices, this->instanceCount);
  else
    glDrawElementsInstanced(
        this->drawMode,
        this->nrOfIndices,
        GL_UNSIGNED_INT,
        0,
        this->instanceCount);

  glBindVertexArray(0);
}

const double Mesh::calculateVolume() const
{
  double volume = 0.0;

  for (size_t i = 0; i < nrOfIndices; i += 3)
  {
    const glm::vec3 &v0 = vertices[indices[i]].position;
    const glm::vec3 &v1 = vertices[indices[i + 1]].position;
    const glm::vec3 &v2 = vertices[indices[i + 2]].position;

    volume += glm::dot(v0, glm::cross(v1, v2));
  }

  return std::abs(volume) / 6.0;
}