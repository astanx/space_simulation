#pragma once

#include "graphics/vertexLayouts.h"
#include "graphics/instanceLayouts.h"
#include "graphics/vertex.h"

#include "graphics/buffers/buffer.h"

#include "graphics/arrays/vertexArray.h"

#include <GL/glew.h>

class Shader;
class Primitive;

template <typename T>
struct TypeTag
{
};

class Mesh
{
private:
  size_t verticesSize;
  size_t indicesSize;

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

  template <typename T>
  void initVAO(std::vector<T> *vertexArray, std::vector<GLuint> *indexArray);

  template <typename T>
  void bindInstanceAttributes(const T *instanceData, const Buffer &vbo) const;

public:
  template <typename T>
  Mesh(std::vector<T> *vertexArray, std::vector<GLuint> *indexArray, VertexLayout layout, GLenum drawMode = GL_TRIANGLES);
  template <typename T>
  Mesh(TypeTag<T>, std::unique_ptr<Primitive> primitive, VertexLayout layout, GLenum drawMode = GL_TRIANGLES);
  ~Mesh() = default;

  template <typename T>
  void updateBuffers(std::vector<T> *vertexArray, std::vector<GLuint> *indexArray);

  template <typename T>
  void setInstanceBuffer(const T *instanceData, size_t count, size_t vboCount = 1);
  template <typename T>
  void updateInstanceBuffer(const T *instanceData, size_t count, size_t vboCount = 1);
  void setInstanceLayout(InstanceLayout layout);

  void render() const;
  void renderInstanced() const;
};

#include "graphics/mesh.tpp"