#pragma once
#define GLEW_STATIC
#include <glew.h>
#include <mat3x3.hpp>
#include <gtc/type_ptr.hpp>
#include "Shader.h"
#include "Texture.h"

class Draw2DShader {
public:
	GLuint shaderID;
	bool texEnabled;
	int  unifOrthro, unifUseTex, unifColor;

	int orthroMatID = -1;

	Draw2DShader(GLuint shader);
	Draw2DShader(const GLchar* vertexPath, const GLchar* fragmentPath);
	~Draw2DShader();
	void SetColor(float r, float g, float b, float a = 1.f);
	void BindTex(GLuint tex = NULL);
};