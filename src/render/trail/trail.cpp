#include "render/trail/trail.h"

#include "debug/logger.h"

#include "camera/camera.h"

#include "graphics/mesh.h"

#include <iostream>
#include <chrono>

// Constructor / Destructor
Trail::Trail(std::unique_ptr<Mesh> trailMesh) : trailMesh(std::move(trailMesh)) {};
Trail::~Trail() = default;

// Public functions
void Trail::addPosition(const glm::vec3 &position)
{
  if (this->positions.size() >= this->maxPositions)
  {
    this->positions.pop_front();
    this->times.pop_front();
  }

  this->positions.push_back(position);
  this->times.push_back(std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count());
}

void Trail::update(const Camera &camera)
{
  if (!this->trailMesh)
    Logger::logFatal("Trail", "Trail mesh not initialized");

  std::vector<VertexPosition> positionsVec;

  double now = std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
  while (!this->positions.empty() && (now - this->times.front()) > this->lifeTime)
  {
    this->positions.pop_front();
    this->times.pop_front();
  }

  for (const glm::vec3 &pos : this->positions)
    positionsVec.emplace_back(camera.worldToViewSpace(pos));

  this->trailMesh->updateBuffers(&positionsVec, nullptr);
}

void Trail::render() const
{
  if (!this->trailMesh)
    Logger::logFatal("Trail", "Trail mesh not initialized");

  this->trailMesh->render();
}