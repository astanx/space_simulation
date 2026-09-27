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
  std::deque<double> times;
  std::unique_ptr<Mesh> trailMesh;
  size_t maxPositions = 2000;
  double lifeTime = 120;

public:
  Trail(std::unique_ptr<Mesh> trailMesh);
  ~Trail();

  void addPosition(const glm::vec3 &position);
  void update(const Camera &camera);
  void render() const;
};