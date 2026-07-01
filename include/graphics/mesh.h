#pragma once

#include "graphics/vertexLayouts.h"
#include "graphics/instanceLayouts.h"
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

  std::unique_ptr<VertexArray> VAO;
  std::unique_ptr<Buffer> VBO;
  std::unique_ptr<Buffer> EBO;

  std::vector<std::unique_ptr<Buffer>> instanceVBOS;
  size_t vboIndex = 0;
  unsigned int instanceCount = 0;
  bool instancingInitialized = false;

  GLenum drawMode;

  LayoutDesc VAOlayout;
  LayoutDesc instanceLayout;

  void initVAO();

  template <typename T>
  void bindInstanceAttributes(const T *instanceData, const Buffer &vbo) const;

public:
  Mesh(std::vector<Vertex> *vertexArray, std::vector<GLuint> *indexArray, VertexLayout layout, GLenum drawMode = GL_TRIANGLES);

  Mesh(std::unique_ptr<Primitive> primitive, VertexLayout layout, GLenum drawMode = GL_TRIANGLES);

  void updateBuffers(std::vector<Vertex> *vertexArray, std::vector<GLuint> *indexArray);

  template <typename T>
  void setInstanceBuffer(const T *instanceData, size_t count, size_t vboCount = 1);
  template <typename T>
  void updateInstanceBuffer(const T *instanceData, size_t count, size_t vboCount = 1);
  void setInstanceLayout(InstanceLayout layout);

  Mesh(const Mesh &obj);
  ~Mesh();

  void render() const;
  void renderInstanced() const;

  const double calculateVolume() const;
};

#include "graphics/mesh.tpp"