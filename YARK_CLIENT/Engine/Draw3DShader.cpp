#include "Draw3DShader.h"
#include <gtc/type_ptr.hpp>

Draw3DShader::Draw3DShader(GLuint shader) {
	shaderID = shader;
	glUseProgram(shader);
	unifProj = glGetUniformLocation(shader, "proj");
	unifView = glGetUniformLocation(shader, "view");
	unifModel = glGetUniformLocation(shader, "model");
	unifColor = glGetUniformLocation(shader, "color");
	unifUseTex = glGetUniformLocation(shader, "useTex");
	texEnabled = true;
	glUniform1i(unifUseTex, 1);
	glUniform1i(glGetUniformLocation(shader, "tex"), 0);
	SetModel(glm::mat4(1));
}

void Draw3DShader::SetModel(glm::mat4 model) {
	glUniformMatrix4fv(unifModel, 1, false, glm::value_ptr(model));
}

Draw3DShader::Draw3DShader(const GLchar* vertexPath, const GLchar* fragmentPath) {
	Draw3DShader(LoadRawShader(vertexPath, fragmentPath));
}

Draw3DShader::~Draw3DShader() {
	glDeleteProgram(shaderID);
}

void Draw3DShader::SetColor(float r, float g, float b, float a) {
	glUniform4f(unifColor, r, g, b, a);
}
void Draw3DShader::BindTex(GLuint tex) {
	bool enabled = tex != 0;
	if (texEnabled != enabled) {
		glUniform1i(unifUseTex, enabled);
	}
	if (enabled) {
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, tex);
	}
	texEnabled = enabled;
}