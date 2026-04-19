#pragma once

class Updatable
{
protected:
public:
  Updatable() = default;
  virtual ~Updatable() = default;

  virtual void update(double dt) = 0;
};