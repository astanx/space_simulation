#include "graphics/primitives/ellipsoid.h"

#include "graphics/vertex.h"

#include "physics/structs/radii.h"

#include <vector>
#include <cmath>

// Prviate function
void Ellipsoid::computeTangents(std::vector<Vertex> &vertices, const std::vector<GLuint> &indices)
{
  std::vector<glm::vec3> tan(vertices.size(), glm::vec3(0));
  std::vector<glm::vec3> bitan(vertices.size(), glm::vec3(0));

  for (size_t i = 0; i < indices.size(); i += 3)
  {
    GLuint i0 = indices[i];
    GLuint i1 = indices[i + 1];
    GLuint i2 = indices[i + 2];

    const glm::vec3 &p0 = vertices[i0].position;
    const glm::vec3 &p1 = vertices[i1].position;
    const glm::vec3 &p2 = vertices[i2].position;

    const glm::vec2 &uv0 = vertices[i0].texcoord;
    const glm::vec2 &uv1 = vertices[i1].texcoord;
    const glm::vec2 &uv2 = vertices[i2].texcoord;

    glm::vec3 edge1 = p1 - p0;
    glm::vec3 edge2 = p2 - p0;

    glm::vec2 deltaUV1 = uv1 - uv0;
    glm::vec2 deltaUV2 = uv2 - uv0;

    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    glm::vec3 T = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
    glm::vec3 B = f * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);

    tan[i0] += T;
    tan[i1] += T;
    tan[i2] += T;

    bitan[i0] += B;
    bitan[i1] += B;
    bitan[i2] += B;
  }

  for (size_t i = 0; i < vertices.size(); ++i)
  {
    glm::vec3 N = vertices[i].normal;
    glm::vec3 T = tan[i];

    // Gram-Schmidt orthogonalization
    T = glm::normalize(T - N * glm::dot(N, T));

    float handedness =
        (glm::dot(glm::cross(N, T), bitan[i]) < 0.0f) ? -1.0f : 1.0f;

    vertices[i].tangent = glm::vec4(T, handedness);
  }
}

// Constructor
Ellipsoid::Ellipsoid(unsigned segments, Radii radii, bool tangent) : Primitive()
{
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;

  glm::vec3 topPos(0.f, radii.polar, 0.f);
  glm::vec3 topNormal(0.f, 1 / radii.polar, 0.f);
  vertices.push_back(Vertex{
      topPos,
      glm::vec3(1.f),
      glm::vec2(0.5f, 0.f),
      glm::normalize(topNormal)});

  for (unsigned i = 1; i < segments; ++i)
  {
    float v = float(i) / float(segments);
    float phi = v * M_PI;

    for (unsigned j = 0; j <= segments; ++j)
    {
      float u = float(j) / float(segments);
      float theta = u * 2.f * M_PI;

      glm::vec3 pos(
          radii.equatorian * sin(phi) * cos(theta),
          radii.polar * cos(phi),
          radii.equatorian * sin(phi) * sin(theta));

      glm::vec3 normal(
          pos.x / (radii.equatorian * radii.equatorian),
          pos.y / (radii.polar * radii.polar),
          pos.z / (radii.equatorian * radii.equatorian));
      normal = glm::normalize(normal);

      vertices.push_back(Vertex{
          pos,
          glm::vec3(1.f),
          glm::vec2(u, v),
          normal});
    }
  }

  glm::vec3 bottomPos(0.f, -radii.polar, 0.f);
  glm::vec3 bottomNormal(0.f, -1 / radii.polar, 0.f);
  vertices.push_back(Vertex{
      bottomPos,
      glm::vec3(1.f),
      glm::vec2(0.5f, 1.f),
      glm::normalize(bottomNormal)});

  unsigned ringVertices = segments + 1;
  unsigned top = 0;
  unsigned bottom = vertices.size() - 1;

  for (unsigned j = 0; j < segments; ++j)
  {
    indices.push_back(top);
    indices.push_back(1 + j + 1);
    indices.push_back(1 + j);
  }

  for (unsigned i = 0; i < segments - 2; ++i)
  {
    for (unsigned j = 0; j < segments; ++j)
    {
      unsigned first = 1 + i * ringVertices + j;
      unsigned second = first + ringVertices;

      indices.push_back(first);
      indices.push_back(first + 1);
      indices.push_back(second);

      indices.push_back(second);
      indices.push_back(first + 1);
      indices.push_back(second + 1);
    }
  }

  unsigned base = bottom - ringVertices;
  for (unsigned j = 0; j < segments; ++j)
  {
    indices.push_back(bottom);
    indices.push_back(base + j);
    indices.push_back(base + j + 1);
  }

  if (tangent)
    this->computeTangents(vertices, indices);

  this->set(vertices.data(), vertices.size(), indices.data(), indices.size());
}
