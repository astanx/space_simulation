#pragma once

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <concepts>

template <typename T>
concept VertexInitable =
    requires(T v, glm::vec3 p, glm::vec2 uv, glm::vec3 n, glm::vec3 c, glm::vec4 t) {
      {
        v.init(p, uv, n, c, t)
      } -> std::same_as<void>;
    };

class Primitive
{
protected:
  std::vector<glm::vec3> positions;
  std::vector<glm::vec2> texcoords;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec3> tangents;
  std::vector<glm::vec3> bitangents;
  std::vector<glm::vec3> colors;
  std::vector<GLuint> indices;

  void computeTangents();

public:
  Primitive();
  virtual ~Primitive();

  template <VertexInitable T>
  std::vector<T> getVertices();
  std::vector<GLuint> &getIndices();
  const double calculateVolume() const;
};

#include "graphics/primitives/primitives.tpp"