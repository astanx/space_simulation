#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <glm/glm.hpp>

class Camera;
struct Trail;
struct Entity;

class TrailManager
{
private:
  std::unordered_map<size_t, size_t> entityToTrailIndex;
  std::vector<std::unique_ptr<Trail>> trails;

public:
  TrailManager();
  ~TrailManager();

  void registerTrail(const Entity entity);
  void addTrailPosition(const Entity entity, const glm::vec3 &position);
  void updateTrail(const Entity entity, const Camera &camera);

  const std::vector<std::unique_ptr<Trail>> &getTrails() const { return this->trails; };
};