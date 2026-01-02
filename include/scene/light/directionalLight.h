#include "scene/light/light.h"

class DirectionalLight : public Light
{
protected:
  glm::vec3 direction;

public:
  DirectionalLight(glm::vec3 direction, glm::vec3 ambient,
                   glm::vec3 diffuse,
                   glm::vec3 specular, float intensity = 1.f);
  ~DirectionalLight() = default;

  void sendToShader(Shader &program);
  void changeDirection(glm::vec3 direction);
};