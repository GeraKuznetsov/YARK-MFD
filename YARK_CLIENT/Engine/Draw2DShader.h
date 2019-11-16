#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/mat3x3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Texture.h"

class Draw2DShader {
public:
	GLuint shaderID;
	bool texEnabled;
	int  unifOrthro, unifUseTex, unifColor, unifView;

	int orthroMatID = -1;

	Draw2DShader(GLuint shader);
	Draw2DShader(const GLchar* vertexPath, const GLchar* fragmentPath);
	~Draw2DShader();
	void SetView(glm::mat4 view);
	void SetColor(float r, float g, float b, float a = 1.f);
	void BindTex(GLuint tex = NULL);
};
