#include "graphics/state/scopedShader.h"

ScopedShader::ScopedShader(const Shader &shader, bool saveState)
{
  if (saveState)
    glGetIntegerv(GL_CURRENT_PROGRAM, &this->prevShader);

  shader.use();
};
ScopedShader::ScopedShader(const GLint program, bool saveState)
{
  if (saveState)
    glGetIntegerv(GL_CURRENT_PROGRAM, &this->prevShader);

  glUseProgram(program);
}

ScopedShader::~ScopedShader()
{
  glUseProgram(this->prevShader);
}