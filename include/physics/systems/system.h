#pragma once

#include <functional>

class Object;

class System
{
protected:
public:
  System() = default;
  virtual ~System() = default;

  virtual void forEachObject(std::function<void(Object &)> &&func) = 0;
};