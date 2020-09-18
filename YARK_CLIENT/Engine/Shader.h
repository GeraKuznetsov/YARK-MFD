#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>

GLuint LoadSHader(const GLchar* vertexPath, const GLchar* fragmentPath);

void SetShaderMat4(GLuint unif, glm::mat4 matrix);
