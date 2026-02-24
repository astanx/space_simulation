#pragma once

#include <string>

class Shader;
class Texture;

class Material
{
protected:
  void sendTexture(Texture *texture, Shader &program, unsigned int bindingPoint, const std::string &name);

public:
  virtual ~Material() = default;

  virtual void sendToShader(Shader &program) = 0;
};
