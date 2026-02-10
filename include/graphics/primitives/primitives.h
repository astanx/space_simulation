#pragma once

#include <vector>
#include <GL/glew.h>

struct Vertex;

class Primitive
{
private:
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;

public:
  Primitive();
  virtual ~Primitive();

  void set(const Vertex *vertices, const unsigned nrOfVertices, const GLuint *indices, const unsigned nrOfIndices);

  Vertex *getVertices();
  GLuint *getIndices();
  const unsigned getNrOfVertices();
  const unsigned getNrOfIndices();
};