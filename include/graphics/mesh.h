#pragma once

#include "graphics/vertexLayouts.h"
#include "graphics/vertex.h"

#include <GL/glew.h>

class Shader;
class Primitive;

class Mesh
{
private:
  Vertex *vertices;
  unsigned nrOfVertices;
  GLuint *indices;
  unsigned nrOfIndices;

  std::unordered_map<VertexLayout, GLuint> VAOS;
  GLuint VBO = 0;
  GLuint EBO = 0;

  GLuint instanceVBO = 0;
  unsigned int instanceCount = 0;
  bool instancingInitialized = false;

  GLenum drawMode;
  VertexLayout layout;

  void initVAO();

public:
  Mesh(Vertex *vertexArray, const unsigned &nrOfVertices, GLuint *indexArray, const unsigned &nrOfIndices, VertexLayout layout, GLenum drawMode = GL_TRIANGLES);

  Mesh(std::unique_ptr<Primitive> primitive, VertexLayout layout, GLenum drawMode = GL_TRIANGLES);

  void updateBuffers(Vertex *vertexArray, const unsigned &nrOfVertices, GLuint *indexArray, const unsigned &nrOfIndices);

  void setInstanceBuffer(const std::vector<InstanceData> &instanceData);
  void updateInstanceBuffer(const std::vector<InstanceData> &instanceData);

  Mesh(const Mesh &obj);
  ~Mesh();

  void render() const;
  void renderInstanced() const;

  const double calculateVolume() const;
};