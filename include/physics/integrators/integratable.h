#pragma once

class Integratable
{
protected:
  bool isSystem;

public:
  Integratable(bool isSystem = false) : isSystem(isSystem) {}
  virtual ~Integratable() = default;

  bool getIsSystem() const { return this->isSystem; }
};