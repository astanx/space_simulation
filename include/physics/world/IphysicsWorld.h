#pragma once

class Star;

class IPhysicsWorld
{
public:
  virtual ~IPhysicsWorld() = 0;
  virtual const Star &getSun() const = 0;
};