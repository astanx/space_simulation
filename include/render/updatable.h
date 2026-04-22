#pragma once

struct Frustum;

class Updatable
{
protected:
public:
  Updatable() = default;
  virtual ~Updatable() = default;

  virtual void update(double dt, Frustum* frustum = nullptr, bool force = false) = 0;
};