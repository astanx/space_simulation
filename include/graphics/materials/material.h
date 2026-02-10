#pragma once

class Shader;
class Texture;

class Material
{
protected:
public:
  virtual ~Material() = default;

  virtual void sendToShader(Shader &program) = 0;
};
