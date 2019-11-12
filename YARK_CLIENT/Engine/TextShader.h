#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/mat3x3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"

class TextShader {
public:
	GLuint shaderID;
	int unifColor, unifOrthro, unifview;
	int orthroMatID = -1;

	TextShader(GLuint shader);
	TextShader(const GLchar* vertexPath, const GLchar* fragmentPath);
	~TextShader();
	void SetView(glm::mat4 view);
	void SetColor(float r, float g, float b, float a = 1.f);
};
