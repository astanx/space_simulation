#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

class Shader;
class Primitive;
struct Vertex;

enum class VertexLayout
{
  Full,
  NoColor,
  PositionOnly,
  Instanced
};

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

  std::unordered_map<VertexLayout, GLuint> VAOS;
  GLuint VBO;
  GLuint EBO;

  GLuint instanceVBO = 0;
  unsigned int instanceCount = 0;
  bool instancingInitialized = false;

  GLenum drawMode;
  VertexLayout layout;

  void initVAO();

public:
  Mesh(Vertex *vertexArray, const unsigned &nrOfVertices, GLuint *indexArray, const unsigned &nrOfIndices, VertexLayout layout, GLenum drawMode = GL_TRIANGLES);

  Mesh(std::unique_ptr<Primitive> primitive, VertexLayout layout, GLenum drawMode = GL_TRIANGLES);

  void setInstancedBuffer(const std::vector<InstanceData> &instancedData);

  Mesh(const Mesh &obj);
  ~Mesh();

  void render();
  void renderInstanced();
};