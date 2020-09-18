#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <vector>
#include <string>

GLuint loadTexture(std::string filename, bool linear = true);
