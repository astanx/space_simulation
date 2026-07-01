#include "graphics/primitives/primitives.h"

#include "graphics/vertex.h"

// Private functions
void Primitive::computeTangents()
{
  this->tangents.resize(this->positions.size(), glm::vec3(0));
  this->bitangents.resize(this->positions.size(), glm::vec3(0));

  for (size_t i = 0; i < indices.size(); i += 3)
  {
    GLuint i0 = indices[i];
    GLuint i1 = indices[i + 1];
    GLuint i2 = indices[i + 2];

    const glm::vec3 &p0 = this->positions[i0];
    const glm::vec3 &p1 = this->positions[i1];
    const glm::vec3 &p2 = this->positions[i2];

    const glm::vec2 &uv0 = this->texcoords[i0];
    const glm::vec2 &uv1 = this->texcoords[i1];
    const glm::vec2 &uv2 = this->texcoords[i2];

    glm::vec3 edge1 = p1 - p0;
    glm::vec3 edge2 = p2 - p0;

    glm::vec2 deltaUV1 = uv1 - uv0;
    glm::vec2 deltaUV2 = uv2 - uv0;

    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    glm::vec3 T = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
    glm::vec3 B = f * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);

    this->tangents[i0] += T;
    this->tangents[i1] += T;
    this->tangents[i2] += T;

    this->bitangents[i0] += B;
    this->bitangents[i1] += B;
    this->bitangents[i2] += B;
  }

  for (size_t i = 0; i < this->tangents.size(); ++i)
  {
    glm::vec3 N = this->normals[i];
    glm::vec3 T = this->tangents[i];

    // Gram-Schmidt orthogonalization
    T = glm::normalize(T - N * glm::dot(N, T));

    float handedness =
        (glm::dot(glm::cross(N, T), this->bitangents[i]) < 0.0f) ? -1.0f : 1.0f;

    this->tangents[i] = glm::vec4(T, handedness);
  }
}

// Constructor and Destructor
Primitive::Primitive() {}
Primitive::~Primitive() {}

// Getters
std::vector<GLuint> &Primitive::getIndices()
{
  return this->indices;
}

const double Primitive::calculateVolume() const
{
  double volume = 0.0;

  for (size_t i = 0; i < this->indices.size(); i += 3)
  {
    const glm::vec3 &v0 = this->positions[this->indices[i]];
    const glm::vec3 &v1 = this->positions[this->indices[i + 1]];
    const glm::vec3 &v2 = this->positions[this->indices[i + 2]];

    volume += glm::dot(v0, glm::cross(v1, v2));
  }

  return std::abs(volume) / 6.0;
}