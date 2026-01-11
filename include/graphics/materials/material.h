#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

class Shader;
class Texture;

class Material
{
protected:
public:
  virtual ~Material() = default;

  virtual void sendToShader(Shader &program) = 0;
};
