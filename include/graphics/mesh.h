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

  GLenum drawMode;

  void initVAO();

public:
  Mesh(Vertex *vertexArray, const unsigned &nrOfVertices, GLuint *indexArray, const unsigned &nrOfIndices, GLenum drawMode = GL_TRIANGLES);

  Mesh(std::unique_ptr<Primitive> primitive, GLenum drawMode = GL_TRIANGLES);

  Mesh(const Mesh &obj);
  ~Mesh();

  void render(Shader *shader);
};