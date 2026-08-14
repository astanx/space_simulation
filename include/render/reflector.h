#pragma once

#include "graphics/model.h"

class ReflectanceAcceptor;

class Reflector : public Model
{
private:
  ReflectanceAcceptor *acceptor;

public:
  using Model::Model;

  void setAcceptor(ReflectanceAcceptor *acceptor) { this->acceptor = acceptor; };
  ReflectanceAcceptor *getAcceptor() { return this->acceptor; };

  ~Reflector() = default;
};