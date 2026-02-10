#include "physics/trail.h"

#include "graphics/vertex.h"
#include "graphics/shader.h"

// Private functions
void Trail::generateTrail(const std::vector<glm::dvec3> &trailVec)
{
  std::vector<Vertex> vertices;
  for (const glm::dvec3 &p : trailVec)
  {
    Vertex v{};
    v.position = p;
    v.color = glm::vec3(1.f);
    v.texcoord = glm::vec2(0.0f);
    v.normal = glm::vec3(0.0f);

    vertices.push_back(v);
  }
  this->trail = std::make_unique<Mesh>(vertices.data(), vertices.size(), nullptr, 0, VertexLayout::PositionOnly, GL_LINE_LOOP);
};

// Constructor
Trail::Trail(const std::vector<glm::dvec3> &trailVec)
{
  this->generateTrail(trailVec);
}

// Public functions
void Trail::render() const
{
  this->trail->render();
}