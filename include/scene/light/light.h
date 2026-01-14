#pragma once

#include <glm/glm.hpp>

class Shader;

class Light
{
protected:
  float intensity;
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;

public:
  Light(glm::vec3 ambient,
        glm::vec3 diffuse,
        glm::vec3 specular, float intensity = 1.f);

  ~Light();

  virtual void sendToShader(Shader &program) = 0;

  inline float getIntensity() const { return this->intensity; }
  inline const glm::vec3 &getAmbient() const { return this->ambient; }
  inline const glm::vec3 &getDiffuse() const { return this->diffuse; }
  inline const glm::vec3 &getSpecular() const { return this->specular; }
};
