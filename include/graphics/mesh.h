#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

class Shader;
class Primitive;
struct Vertex;

struct InstanceData
{
  glm::mat4 ModelMatrix;
};

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

  GLuint instanceVBO = 0;
  unsigned int instanceCount = 0;
  bool instancingInitialized = false;

  GLenum drawMode;

  void initVAO();

public:
  Mesh(Vertex *vertexArray, const unsigned &nrOfVertices, GLuint *indexArray, const unsigned &nrOfIndices, GLenum drawMode = GL_TRIANGLES);

  Mesh(std::unique_ptr<Primitive> primitive, GLenum drawMode = GL_TRIANGLES);

  void setInstancedBuffer(const std::vector<InstanceData> &instancedData);

  Mesh(const Mesh &obj);
  ~Mesh();

  void render();
  void renderInstanced();
};