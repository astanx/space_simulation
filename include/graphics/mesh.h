#pragma once

#include "graphics/vertexLayouts.h"
#include "graphics/vertex.h"

#include "graphics/buffers/buffer.h"

#include "graphics/arrays/vertexArray.h"

#include <GL/glew.h>

class Shader;
class Primitive;

class Mesh
{
private:
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;

  std::unordered_map<VertexLayout, std::unique_ptr<VertexArray>> VAOS;
  std::unique_ptr<Buffer> VBO;
  std::unique_ptr<Buffer> EBO;

  std::unique_ptr<Buffer> instanceVBO;
  unsigned int instanceCount = 0;
  bool instancingInitialized = false;

  GLenum drawMode;
  VertexLayout layout;

  bool layoutInit = false;

  void initVAO();

public:
  Mesh(std::vector<Vertex> *vertexArray, std::vector<GLuint> *indexArray, VertexLayout layout, GLenum drawMode = GL_TRIANGLES);

  Mesh(std::unique_ptr<Primitive> primitive, VertexLayout layout, GLenum drawMode = GL_TRIANGLES);

  void updateBuffers(std::vector<Vertex> *vertexArray, std::vector<GLuint> *indexArray);

  void setInstanceBuffer(const std::vector<InstanceData> &instanceData);
  void updateInstanceBuffer(const std::vector<InstanceData> &instanceData);

  Mesh(const Mesh &obj);
  ~Mesh();

  void render() const;
  void renderInstanced() const;

  const double calculateVolume() const;
};