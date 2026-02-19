#pragma once

#include <string>
#include <unordered_map>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Shader
{
private:
  GLuint id;
  std::unordered_map<std::string, GLint> uniformCache;
  int GLSLmajor;
  int GLSLminor;

  std::string loadShaderSrc(const char *fileName, bool isInclude = false);

  GLuint loadShader(GLenum type, char *fileName);

  void linkProgram(GLuint vertexShader, GLuint geometryShader, GLuint fragmentShader);

  GLint getUniformLocation(const std::string &name);

public:
  Shader(const int GLSLmajor, const int GLSLminor, const char *vertexFile, const char *fragmentFile, const char *geometryFile = nullptr);

  ~Shader();

  inline GLuint &getId() { return this->id; }

  void use() const;
  void unuse() const;

  void set1i(GLint value, const std::string &name);

  void set1f(GLfloat value, const std::string &name);

  void setVec1f(const GLfloat *value, GLsizei size, const std::string &name);

  void setVec2f(glm::fvec2 value, const std::string &name);

  void setVec3f(glm::fvec3 value, const std::string &name);

  void setVec4f(glm::fvec4 value, const std::string &name);

  void setMat3fv(glm::mat3 value, const std::string &name, GLboolean transpose = GL_FALSE);

  void setMat4fv(glm::mat4 value, const std::string &name, GLboolean transpose = GL_FALSE);
};