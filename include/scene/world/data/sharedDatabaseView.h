#pragma once

#include "camera/camera.h"

#include "physics/structs/radii.h"
#include "resources/transform.h"

#include "scene/light/pointLight.h"

#include "resources/transform.h"

#include "physics/structs/radii.h"

#include "scene/world/data/sharedDatabase.h"
#include "scene/world/data/sharedGPUBuffers.h"
#include "scene/world/data/IsharedDatabaseView.h"

class PointLight;
class Camera;
class CLBuffer;

template <typename Real>
struct SharedDatabaseView : public ISharedDatabaseView
{
  const SharedDatabase<Real> &database;
  const SharedGPUBuffers &gpu;

  SharedDatabaseView(const SharedDatabase<Real> &database, const SharedGPUBuffers &gpu) : database(database), gpu(gpu) {}
  ~SharedDatabaseView() = default;

  glm::vec3 getPosition(size_t idx) const override
  {
    return this->database.positions[idx];
  }

  Transform getTransform(size_t idx, const Camera &camera) const override
  {
    return Transform{camera.worldToViewSpace(this->database.positions[idx]), camera.worldToViewSpace(this->database.orientations[idx])};
  }

  Radii getRadii(size_t idx) const override
  {
    return Radii{this->database.equatorianRadii[idx], this->database.polarRadii[idx], this->database.meanRadii[idx]};
  }

  void moveLight(size_t idx, const Camera &camera, PointLight &light) const override
  {
    light.move(camera.worldToViewSpace(this->database.positions[idx]));
  }

  const CLBuffer &getPositionsBuffer() const override
  {
    return this->gpu.positionsBuffer;
  }

  size_t getPositionsSize() const override
  {
    return sizeof(Vec3<Real>);
  }
};