#include "physics/trail.h"

#include "camera/camera.h"

#include "render/modelSource.h"

#include "graphics/vertex.h"
#include "graphics/shader.h"

// Private functions
void Trail::generateTrail(const Camera &camera)
{
  std::vector<Vertex> vertices;
  for (const glm::dvec3 &p : trailVec)
  {
    Vertex v{};
    v.position = camera.worldToViewSpace(p);
    v.color = glm::vec3(1.f);
    v.texcoord = glm::vec2(0.0f);
    v.normal = glm::vec3(0.0f);

    vertices.push_back(v);
  }

  if (!this->trail)
    this->trail = std::make_unique<Mesh>(&vertices, nullptr, VertexLayout::PositionOnly, GL_LINE_LOOP);
  else
    this->trail->updateBuffers(&vertices, nullptr);
};

// Constructor
Trail::Trail(const std::vector<glm::dvec3> &trailVec) : trailVec(trailVec)
{}

// Public functions
void Trail::render() const
{
  this->trail->render();
}

void Trail::update(const Camera &camera)
{
  this->generateTrail(camera);
}