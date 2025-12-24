#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

class Shader;
class Primitive;
struct Vertex;

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

  void initVAO(Primitive *primitive);

  void initVAO(Vertex *vertexArray, const unsigned &nrOfVertices, GLuint *indexArray, const unsigned &nrOfIndices);

  void updateUniforms(Shader *shader);

  void updateModelMatrix();

public:
  Mesh(
      Vertex *vertexArray, const unsigned &nrOfVertices, GLuint *indexArray, const unsigned &nrOfIndices,
      glm::vec3 position = glm::vec3(0.f), glm::vec3 rotation = glm::vec3(0.f), glm::vec3 scale = glm::vec3(1.f));

  Mesh(
      Primitive *primitive,
      glm::vec3 position = glm::vec3(0.f), glm::vec3 rotation = glm::vec3(0.f), glm::vec3 scale = glm::vec3(1.f));
  ~Mesh();

  // Setters
  void setPosition(const glm::vec3 position);

  void setRotation(const glm::vec3 rotation);

  void setScale(const glm::vec3 scale);

  // Functions
  void move(const glm::vec3 position);
  void rotate(const glm::vec3 rotation);
  void scaleBy(const glm::vec3 scale);

  void render(Shader *shader);
};