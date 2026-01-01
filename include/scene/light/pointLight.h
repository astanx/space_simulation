#include "scene/light/light.h"

class PointLight : public Light
{
protected:
  glm::vec3 position;
  float constant;
  float linear;
  float quadratic;

public:
  PointLight(glm::vec3 position, glm::vec3 ambient,
             glm::vec3 diffuse,
             glm::vec3 specular, float intensity = 1.f,
             float constant = 1.f, float linear = 0.045f, float quadratic = 0.0075f);
  ~PointLight() = default;

  void sendToShader(Shader &program);
  void move(glm::vec3 position);
};