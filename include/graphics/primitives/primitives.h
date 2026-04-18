#pragma once

#include <vector>
#include <GL/glew.h>

struct Vertex;

class Primitive
{
protected:
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;

  void computeTangents(std::vector<Vertex> &vertices, const std::vector<GLuint> &indices);

public:
  Primitive();
  virtual ~Primitive();

  void set(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices);

  std::vector<Vertex> &getVertices();
  std::vector<GLuint> &getIndices();
};