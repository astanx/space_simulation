#pragma once

struct Entity;

class IPhysicsWorld
{
public:
  virtual ~IPhysicsWorld() = 0;
  virtual const Entity &getSun() const = 0;
};