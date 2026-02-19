#include "graphics/shader.h"

#include "debug/logger.h"

#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>

// Private functions
std::string Shader::loadShaderSrc(const char *fileName, bool isInclude)
{
  std::string temp = "";
  std::string src = "";

  std::ifstream inFile;

  inFile.open(fileName);

  if (inFile.is_open())
  {
    while (std::getline(inFile, temp))
    {
      if (temp.find("#version") == 0)
        continue;

      if (temp.find("#include") == 0)
      {
        long start = temp.find("\"") + 1;
        long end = temp.find("\"", start);
        std::string includePath = temp.substr(start, end - start);
        src += this->loadShaderSrc(("assets/shaders/" + includePath).c_str(), true) + "\n";
      }
      else
        src += temp + "\n";
    }
  }
  else
    std::cerr << "[Shader] RUNTIME ERROR: Could not open shader file - " << fileName << std::endl;

  inFile.close();

  if (!isInclude)
  {
    std::string versionLine = "#version " + std::to_string(this->GLSLmajor) + std::to_string(this->GLSLminor) + "0\n";

    src = versionLine + src;
  }

  return src;
}

GLuint Shader::loadShader(GLenum type, char *fileName)
{
  char infoLog[512];
  GLint success;

  GLuint shader = glCreateShader(type);
  std::string str_src = this->loadShaderSrc(fileName);
  const GLchar *src = str_src.c_str();
  glShaderSource(shader, 1, &src, NULL);
  GL_CALL(glCompileShader(shader));

  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  if (!success)
  {
    std::cerr << "[Shader] RUNTIME ERROR: Compilation failed for - " << fileName << std::endl;
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cout << infoLog << std::endl;
  }
  return shader;
}

void Shader::linkProgram(GLuint vertexShader, GLuint geometryShader, GLuint fragmentShader)
{
  char infoLog[512];
  GLint success;

  this->id = glCreateProgram();
  if (!this->id)
    Logger::logFatal("Shader", "Failed to create program");

  glAttachShader(this->id, vertexShader);
  if (geometryShader)
    glAttachShader(this->id, geometryShader);
  glAttachShader(this->id, fragmentShader);

  GL_CALL(glLinkProgram(this->id));

  glGetProgramiv(this->id, GL_LINK_STATUS, &success);
  if (!success)
  {
    Logger::logFatal("Shader", "Failed to link program");
    glGetProgramInfoLog(this->id, 512, NULL, infoLog);
    std::cout << infoLog << std::endl;
  }

  glUseProgram(0);
}

GLint Shader::getUniformLocation(const std::string &name)
{
  auto it = uniformCache.find(name);
  if (it != uniformCache.end())
    return it->second;

  GLint loc = glGetUniformLocation(this->id, name.c_str());
  uniformCache[name] = loc;
  return loc;
}

// Constructor / Destructor
Shader::Shader(const int GLSLmajor, const int GLSLminor, const char *vertexFile, const char *fragmentFile, const char *geometryFile)
{
  this->GLSLmajor = GLSLmajor;
  this->GLSLminor = GLSLminor;
  GLuint vertexShader = 0;
  GLuint geometryShader = 0;
  GLuint fragmentShader = 0;

  vertexShader = loadShader(GL_VERTEX_SHADER, const_cast<char *>(vertexFile));
  if (geometryFile != nullptr)
    geometryShader = loadShader(GL_GEOMETRY_SHADER, const_cast<char *>(geometryFile));
  fragmentShader = loadShader(GL_FRAGMENT_SHADER, const_cast<char *>(fragmentFile));

  this->linkProgram(vertexShader, geometryShader, fragmentShader);

  glDeleteShader(vertexShader);
  glDeleteShader(geometryShader);
  glDeleteShader(fragmentShader);
}

Shader::~Shader()
{
  glDeleteProgram(this->id);
}

// Setters
void Shader::set1i(GLint value, const std::string &name)
{
  GLint position = getUniformLocation(name);
  if (position == -1)
    return;
  glUniform1i(position, value);
}

void Shader::set1f(GLfloat value, const std::string &name)
{
  GLint position = getUniformLocation(name);
  if (position == -1)
    return;
  glUniform1f(position, value);
}

void Shader::setVec1f(const GLfloat *value, GLsizei size, const std::string &name)
{
  GLint position = getUniformLocation(name);
  if (position == -1)
    return;
  glUniform1fv(position, size, value);
}

void Shader::setVec2f(glm::fvec2 value, const std::string &name)
{
  GLint position = getUniformLocation(name);
  if (position == -1)
    return;
  glUniform2fv(position, 1, glm::value_ptr(value));
}

void Shader::setVec3f(glm::fvec3 value, const std::string &name)
{
  GLint position = getUniformLocation(name);
  if (position == -1)
    return;
  glUniform3fv(position, 1, glm::value_ptr(value));
}

void Shader::setVec4f(glm::fvec4 value, const std::string &name)
{
  GLint position = getUniformLocation(name);
  if (position == -1)
    return;
  glUniform4fv(position, 1, glm::value_ptr(value));
}

void Shader::setMat3fv(glm::mat3 value, const std::string &name, GLboolean transpose)
{
  GLint position = getUniformLocation(name);
  if (position == -1)
    return;
  glUniformMatrix3fv(position, 1, transpose, glm::value_ptr(value));
}

void Shader::setMat4fv(glm::mat4 value, const std::string &name, GLboolean transpose)
{
  GLint position = getUniformLocation(name);
  if (position == -1)
    return;
  glUniformMatrix4fv(position, 1, transpose, glm::value_ptr(value));
}
// Public functions
void Shader::use() const
{
  if (glIsProgram(this->id))
    glUseProgram(this->id);
  else
    Logger::logFatal("Shader", "No program to use");
}

void Shader::unuse() const
{
  glUseProgram(0);
}