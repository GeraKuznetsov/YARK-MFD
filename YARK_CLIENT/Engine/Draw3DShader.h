#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Texture.h"

class Draw3DShader {
public:
	GLuint shaderID;
	int unifView, unifModel, unifProj, unifColor, unifUseTex;
	bool texEnabled;
	int projMatID = -1, viewMatID = -1;

	void SetModel(glm::mat4 model);
	Draw3DShader(GLuint shader);
	Draw3DShader(const GLchar* vertexPath, const GLchar* fragmentPath);
	~Draw3DShader();
	void SetColor(float r, float g, float b, float a = 1.f);
	void BindTex(GLuint tex = NULL);
};
