#pragma once

#include <deque>
#include <vector>
#include <memory>
#include <glm/glm.hpp>

class Mesh;
class Camera;

class Trail
{
private:
  std::deque<glm::vec3> positions;
  std::unique_ptr<Mesh> trailMesh;
  size_t maxPositions = 3000;

public:
  Trail(std::unique_ptr<Mesh> trailMesh);
  ~Trail();

  void addPosition(const glm::vec3 &position);
  void update(const Camera &camera);
  void render() const;
};