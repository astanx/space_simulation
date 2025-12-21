#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL2/SOIL2.h>

#include <OpenGL/gl.h>
#include <GLFW/glfw3.h>


class Light
{
private:
  glm::vec3 position;
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;

  float constant;
  float linear;
  float quadratic;

public:
  Light(
    glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic
  )
  { 
    this->position = position;
    this->ambient = ambient;
    this->diffuse = diffuse;
    this->specular = specular;
    this->constant = constant;
    this->linear = linear;
    this->quadratic = quadratic;
  }

  ~Light(){}

  void sendToShader(Shader& program)
  {
    program.setVec3f(this->position, "light.position");
    program.setVec3f(this->ambient, "light.ambient");
    program.setVec3f(this->diffuse, "light.diffuse");
    program.setVec3f(this->specular, "light.specular");

    program.set1f(this->constant, "light.constant");
    program.set1f(this->linear, "light.linear");
    program.set1f(this->quadratic, "light.quadratic");
  }

  void move(glm::vec3 position)
  {
    this->position = position;
  }
};  

