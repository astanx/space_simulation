#include "graphics/mesh.h"
#include "graphics/shader.h"
#include "graphics/primitives/primitives.h"
#include "graphics/mesh.h"
#include "graphics/vertex.h"

#include <glm/gtc/matrix_transform.hpp>

// Private functions
void Mesh::initVAO()
{
  glGenVertexArrays(1, &this->VAO);
  glBindVertexArray(this->VAO);

  glGenBuffers(1, &this->VBO);
  glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
  glBufferData(GL_ARRAY_BUFFER, this->nrOfVertices * sizeof(Vertex), this->vertices, GL_STATIC_DRAW);

  if (this->nrOfIndices > 0)
  {
    glGenBuffers(1, &this->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->nrOfIndices * sizeof(GLuint), this->indices, GL_STATIC_DRAW);
  }

  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, position));
  glEnableVertexAttribArray(0);

  // Color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, color));
  glEnableVertexAttribArray(1);

  // Texcoord attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, texcoord));
  glEnableVertexAttribArray(2);

  // Normal attribute
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, normal));
  glEnableVertexAttribArray(3);

  glBindVertexArray(0);
}

// Constructor and Destructor
Mesh::Mesh(Vertex *vertexArray, const unsigned &nrOfVertices, GLuint *indexArray, const unsigned &nrOfIndices, GLenum drawMode)
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

  this->initVAO();
}

Mesh::Mesh(std::unique_ptr<Primitive> primitive, GLenum drawMode)
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

  this->initVAO();
}

Mesh::~Mesh()
{
  glDeleteVertexArrays(1, &this->VAO);
  glDeleteBuffers(1, &this->VBO);
  if (this->nrOfIndices > 0)
    glDeleteBuffers(1, &this->EBO);
  if (instancingInitialized)
    glDeleteBuffers(1, &instanceVBO);

  delete[] this->vertices;
  delete[] this->indices;
}

// Functions
void Mesh::setInstancedBuffer(const std::vector<InstanceData> &instancedData)
{
  if (!instancingInitialized)
  {
    glGenBuffers(1, &instanceVBO);
    instancingInitialized = true;
  }

  instanceCount = static_cast<unsigned int>(instancedData.size());

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

  glBufferData(
      GL_ARRAY_BUFFER,
      instanceCount * sizeof(InstanceData),
      instancedData.data(),
      GL_STATIC_DRAW);

  constexpr GLuint INSTANCE_ATTRIB_START = 4;

  for (int i = 0; i < 4; i++)
  {
    glEnableVertexAttribArray(INSTANCE_ATTRIB_START + i);
    glVertexAttribPointer(
        INSTANCE_ATTRIB_START + i,
        4,
        GL_FLOAT,
        GL_FALSE,
        sizeof(InstanceData),
        (void *)(sizeof(glm::vec4) * i));
    glVertexAttribDivisor(INSTANCE_ATTRIB_START + i, 1);
  }

  glBindVertexArray(0);
}

void Mesh::render()
{
  glBindVertexArray(this->VAO);

  if (this->nrOfIndices == 0)
    glDrawArrays(this->drawMode, 0, this->nrOfVertices);
  else
    glDrawElements(this->drawMode, this->nrOfIndices, GL_UNSIGNED_INT, 0);
};

void Mesh::renderInstanced()
{
  if (!instancingInitialized || instanceCount == 0)
    return;

  glBindVertexArray(VAO);

  if (nrOfIndices == 0)
    glDrawArraysInstanced(drawMode, 0, nrOfVertices, instanceCount);
  else
    glDrawElementsInstanced(
        drawMode,
        nrOfIndices,
        GL_UNSIGNED_INT,
        0,
        instanceCount);

  glBindVertexArray(0);
}
