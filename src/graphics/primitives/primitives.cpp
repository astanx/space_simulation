#include "graphics/primitives/primitives.h"

#include "graphics/vertex.h"

// Private functions
void Primitive::computeTangents(std::vector<Vertex> &vertices, const std::vector<GLuint> &indices)
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

// Constructor and Destructor
Primitive::Primitive() {}
Primitive::~Primitive() {}

// Setters
void Primitive::set(const Vertex *vertices, const unsigned nrOfVertices, const GLuint *indices, const unsigned nrOfIndices)
{
  for (size_t i = 0; i < nrOfVertices; i++)
  {
    this->vertices.push_back(vertices[i]);
  }

  for (size_t i = 0; i < nrOfIndices; i++)
  {
    this->indices.push_back(indices[i]);
  }
}

// Getters

Vertex *Primitive::getVertices()
{
  return this->vertices.data();
}
GLuint *Primitive::getIndices()
{
  return this->indices.data();
}
const unsigned Primitive::getNrOfVertices()
{
  return this->vertices.size();
}
const unsigned Primitive::getNrOfIndices()
{
  return this->indices.size();
}