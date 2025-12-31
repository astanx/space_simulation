#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

class Shader;
class Primitive;
struct Vertex;

class Mesh
{
private:
  Vertex *vertices;
  unsigned nrOfVertices;
  GLuint *indices;
  unsigned nrOfIndices;

  GLuint VAO;
  GLuint VBO;
  GLuint EBO;

  glm::vec3 position;
  glm::vec3 rotationOrigin;
  glm::vec3 rotation;
  glm::vec3 scale;
  glm::mat4 ModelMatrix;

  void initVAO();

  void updateUniforms(Shader *shader);

  void updateModelMatrix();

public:
  Mesh(
      Vertex *vertexArray, const unsigned &nrOfVertices, GLuint *indexArray, const unsigned &nrOfIndices,
      glm::vec3 position = glm::vec3(0.f), glm::vec3 rotationOrigin = glm::vec3(0.f), glm::vec3 rotation = glm::vec3(0.f), glm::vec3 scale = glm::vec3(1.f));

  Mesh(
      std::unique_ptr<Primitive> primitive,
      glm::vec3 position = glm::vec3(0.f), glm::vec3 rotationOrigin = glm::vec3(0.f), glm::vec3 rotation = glm::vec3(0.f), glm::vec3 scale = glm::vec3(1.f));

  Mesh(const Mesh &obj);
  ~Mesh();

  // Setters
  void setPosition(const glm::vec3 position);

  void setRotationOrigin(const glm::vec3 rotationOrigin);

  void setRotation(const glm::vec3 rotation);

  void setScale(const glm::vec3 scale);

  // Functions
  void move(const glm::vec3 position);
  void rotate(const glm::vec3 rotation);
  void scaleBy(const glm::vec3 scale);

  void render(Shader *shader);
};