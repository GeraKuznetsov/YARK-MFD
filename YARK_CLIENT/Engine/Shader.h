#pragma once
#define GLEW_STATIC
#include <glew.h>
#include <glm.hpp>

GLuint LoadSHader(const GLchar* vertexPath, const GLchar* fragmentPath);

void SetShaderMat4(GLuint unif, glm::mat4 matrix);