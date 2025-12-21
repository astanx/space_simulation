#include <iostream>
#include <vector>
#include "material.h"
#include "texture.h"
#include "vertex.h"

class Mesh
{
private:
  unsigned nrOfVertices;
  unsigned nrOfIndices;

  GLuint VAO;
  GLuint VBO;
  GLuint EBO;

  glm::vec3 position;
  glm::vec3 rotation;
  glm::vec3 scale;
  glm::mat4 ModelMatrix;

  
  void initVAO(Vertex* vertexArray, const unsigned& nrOfVertices, GLuint* indexArray, const unsigned& nrOfIndices)
  {
    this->nrOfVertices = nrOfVertices;
    this->nrOfIndices = nrOfIndices;


	  glGenVertexArrays(1, &this->VAO);
    glBindVertexArray(this->VAO);

    glGenBuffers(1, &this->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, this->nrOfVertices * sizeof(Vertex), vertexArray, GL_STATIC_DRAW);

    glGenBuffers(1, &this->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->nrOfIndices * sizeof(GLuint), indexArray, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);

    // Texcoord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texcoord));
    glEnableVertexAttribArray(2);

    // Normal attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
  }

  void updateUniforms(Shader* shader)
  {
    shader->setMat4fv(this->ModelMatrix, "ModelMatrix");
  }

  void updateModelMatrix()
  {
    this->ModelMatrix = glm::mat4(1.f);
    this->ModelMatrix = glm::translate(this->ModelMatrix, this->position);
    this->ModelMatrix = glm::rotate(this->ModelMatrix, glm::radians(this->rotation.x),  glm::vec3(1.f, 0.f, 0.f));
    this->ModelMatrix = glm::rotate(this->ModelMatrix, glm::radians(this->rotation.y),  glm::vec3(0.f, 1.f, 0.f));
    this->ModelMatrix = glm::rotate(this->ModelMatrix, glm::radians(this->rotation.z),  glm::vec3(0.f, 0.f, 1.f));
    this->ModelMatrix = glm::scale(this->ModelMatrix, this->scale);
  }
public:
  Mesh(
    Vertex* vertexArray, const unsigned& nrOfVertices, GLuint* indexArray, const unsigned& nrOfIndices,
    glm::vec3 position = glm::vec3(0.f), glm::vec3 rotation = glm::vec3(0.f), glm::vec3 scale = glm::vec3(1.f)
  )
  {
    this->position = position;
    this->rotation = rotation;
    this->scale = scale;
    this->initVAO(vertexArray, nrOfVertices, indexArray, nrOfIndices);
    this->updateModelMatrix();
  }

  ~Mesh()
  {
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteBuffers(1, &this->VBO);
    glDeleteBuffers(1, &this->EBO);
  }

  //Setters
  void setPosition(const glm::vec3 position)
  {
    this->position = position;
  }

  void setRotation(const glm::vec3 rotation)
  {
    this->rotation = rotation;
  }

  void setScale(const glm::vec3 scale)
  {
    this->scale = scale;
  }

  //Functions
  void move(const glm::vec3 position)
  {
    this->position += position;
  }

  void rotate(const glm::vec3 rotation)
  {
    this->rotation += rotation;
  }

  void scaleBy(const glm::vec3 scale)
  {
    this->scale *= scale;
  }

  void update()
  {};

  void render(Shader* shader)
  {
    this->updateModelMatrix();
    this->updateUniforms(shader);

    shader->use();

    glBindVertexArray(this->VAO);
	 
    if (this->nrOfIndices == 0) glDrawArrays(GL_TRIANGLES, 0, this->nrOfVertices);
		else glDrawElements(GL_TRIANGLES, this->nrOfIndices, GL_UNSIGNED_INT, 0);

    shader->unuse();
  };
};