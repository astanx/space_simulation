#include "render/trail/trail.h"

#include "debug/logger.h"

#include "camera/camera.h"

#include "graphics/mesh.h"

#include <iostream>

// Constructor / Destructor
Trail::Trail(std::unique_ptr<Mesh> trailMesh) : trailMesh(std::move(trailMesh)) {};
Trail::~Trail() = default;

// Public functions
void Trail::addPosition(const glm::vec3 &position)
{
  std::cout << "SIZE: " << this->positions.size() << std::endl;
  if (this->positions.size() >= this->maxPositions)
    this->positions.pop_front();
  this->positions.push_back(position);
}

void Trail::update(const Camera &camera)
{
  if (!this->trailMesh)
    Logger::logFatal("Trail", "Trail mesh not initialized");

  std::vector<VertexPosition> positionsVec;
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