#pragma once
#define GLEW_STATIC
#include <glew.h>
#include <vector>
#include <string>

GLuint loadTexture(std::string filename, bool linear = true);