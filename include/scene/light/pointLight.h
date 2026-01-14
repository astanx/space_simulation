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

  void sendToShader(Shader &program) override;
  void move(glm::vec3 position);

  inline const glm::vec3 &getPosition() const { return this->position; }
  inline float getConstant() const { return this->constant; }
  inline float getLinear() const { return this->linear; }
  inline float getQuadratic() const { return this->quadratic; }
};