#include "render/trail/trailManager.h"

#include "debug/logger.h"

#include "render/trail/trail.h"

#include "resources/entity/entity.h"

#include "graphics/vertexLayouts.h"
#include "graphics/mesh.h"

#include <iostream>

// Constructor / Destructor
TrailManager::TrailManager() = default;
TrailManager::~TrailManager() = default;

// Public functions
void TrailManager::registerTrail(const Entity entity)
{
  auto it = this->entityToTrailIndex.find(entity.id);
  if (it != this->entityToTrailIndex.end())
    Logger::logFatal("Trail Manager", "Entity trail registered twice");

  this->entityToTrailIndex[entity.id] = this->trails.size();
  std::vector<VertexPosition> vertices;
  auto mesh = std::make_unique<Mesh>(
      &vertices,
      nullptr,
      VertexLayout::PositionOnly,
      GL_LINE_STRIP);

  this->trails.emplace_back(std::make_unique<Trail>(std::move(mesh)));
}

void TrailManager::addTrailPosition(const Entity entity, const glm::vec3 &position)
{
  auto it = this->entityToTrailIndex.find(entity.id);
  if (it == this->entityToTrailIndex.end())
    Logger::logFatal("Trail Manager", "Entity trail not registered");

  this->trails[it->second]->addPosition(position);
}

void TrailManager::updateTrail(const Entity entity, const Camera &camera)
{
  auto it = this->entityToTrailIndex.find(entity.id);
  if (it == this->entityToTrailIndex.end())
    Logger::logFatal("Trail Manager", "Entity trail not registered");

  this->trails[it->second]->update(camera);
}