#include "mesh.h"
#include "render/shader/shader.h"
#include "render/primitives/primitives.h"
#include "render/vertex/vertex.h"
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

void Mesh::updateUniforms(Shader *shader)
{
  shader->setMat4fv(this->ModelMatrix, "ModelMatrix");
}

void Mesh::updateModelMatrix()
{
  this->ModelMatrix = glm::mat4(1.f);
  this->ModelMatrix = glm::translate(this->ModelMatrix, this->rotationOrigin);
  this->ModelMatrix = glm::rotate(this->ModelMatrix, glm::radians(this->rotation.x), glm::vec3(1.f, 0.f, 0.f));
  this->ModelMatrix = glm::rotate(this->ModelMatrix, glm::radians(this->rotation.y), glm::vec3(0.f, 1.f, 0.f));
  this->ModelMatrix = glm::rotate(this->ModelMatrix, glm::radians(this->rotation.z), glm::vec3(0.f, 0.f, 1.f));
  this->ModelMatrix = glm::translate(this->ModelMatrix, this->position - this->rotationOrigin);
  this->ModelMatrix = glm::scale(this->ModelMatrix, this->scale);
}

// Constructor and Destructor
Mesh::Mesh(
    Vertex *vertexArray, const unsigned &nrOfVertices, GLuint *indexArray, const unsigned &nrOfIndices,
    glm::vec3 position, glm::vec3 rotationOrigin, glm::vec3 rotation, glm::vec3 scale)
{
  this->position = position;
  this->rotationOrigin = rotationOrigin;
  this->rotation = rotation;
  this->scale = scale;

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

  this->initVAO();
  this->updateModelMatrix();
}

Mesh::Mesh(
    Primitive *primitive,
    glm::vec3 position, glm::vec3 rotationOrigin, glm::vec3 rotation, glm::vec3 scale)
{
  this->position = position;
  this->rotationOrigin = rotationOrigin;
  this->rotation = rotation;
  this->scale = scale;

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

  this->initVAO();
  this->updateModelMatrix();
}

Mesh::Mesh(const Mesh &obj)
{
  this->position = obj.position;
  this->rotationOrigin = obj.rotationOrigin;
  this->rotation = obj.rotation;
  this->scale = obj.scale;

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

  this->initVAO();
  this->updateModelMatrix();
}

Mesh::~Mesh()
{
  glDeleteVertexArrays(1, &this->VAO);
  glDeleteBuffers(1, &this->VBO);
  if (this->nrOfIndices > 0)
    glDeleteBuffers(1, &this->EBO);

  delete[] this->vertices;
  delete[] this->indices;
}

// Setters
void Mesh::setPosition(const glm::vec3 position)
{
  this->position = position;
}

void Mesh::setRotationOrigin(const glm::vec3 rotationOrigin)
{
  this->rotationOrigin = rotationOrigin;
}


void Mesh::setRotation(const glm::vec3 rotation)
{
  this->rotation = rotation;
}

void Mesh::setScale(const glm::vec3 scale)
{
  this->scale = scale;
}

// Functions

void Mesh::move(const glm::vec3 position)
{
  this->position += position;
}

void Mesh::rotate(const glm::vec3 rotation)
{
  this->rotation += rotation;
}

void Mesh::scaleBy(const glm::vec3 scale)
{
  this->scale *= scale;
}

void Mesh::render(Shader *shader)
{
  this->updateModelMatrix();
  this->updateUniforms(shader);

  shader->use();

  glBindVertexArray(this->VAO);

  if (this->nrOfIndices == 0)
    glDrawArrays(GL_TRIANGLES, 0, this->nrOfVertices);
  else
    glDrawElements(GL_TRIANGLES, this->nrOfIndices, GL_UNSIGNED_INT, 0);

  shader->unuse();
};