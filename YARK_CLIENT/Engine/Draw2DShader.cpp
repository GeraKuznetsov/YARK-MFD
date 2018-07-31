#include "Draw2DShader.h"
#include <iostream>

Draw2DShader::Draw2DShader(GLuint shader) {
	shaderID = shader;
	glUseProgram(shader);
	unifOrthro = glGetUniformLocation(shader, "orthro");
	unifUseTex = glGetUniformLocation(shader, "useTex");
	unifColor = glGetUniformLocation(shader, "color");
	texEnabled = true;
	glUniform1i(unifUseTex, 1);
	glUniform1i(glGetUniformLocation(shader, "tex"), 0);
}

Draw2DShader::Draw2DShader(const GLchar* vertexPath, const GLchar* fragmentPath) {
	Draw2DShader(LoadRawShader(vertexPath, fragmentPath));
}

Draw2DShader::~Draw2DShader() {
	glDeleteProgram(shaderID);
}

void Draw2DShader::SetColor(float r, float g, float b, float a) {
	glUniform4f(unifColor, r, g, b, a);
}

void Draw2DShader::BindTex(GLuint tex) {
	bool enabled = tex != 0;
	if (texEnabled != enabled) {
		glUniform1i(unifUseTex, enabled);
	}
	if (enabled) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
	}
	texEnabled = enabled;
}