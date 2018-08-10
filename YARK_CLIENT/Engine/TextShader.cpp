#include "TextShader.h"
#include <iostream>

TextShader::TextShader(GLuint shader) {
	shaderID = shader;
	glUseProgram(shader);
	unifColor = glGetUniformLocation(shader, "color");
	unifOrthro = glGetUniformLocation(shader, "orthro");
	unifview = glGetUniformLocation(shader, "view");
	glUniform1i(glGetUniformLocation(shader, "tex"), 0);
	SetView(glm::mat4(1));
}

TextShader::TextShader(const GLchar* vertexPath, const GLchar* fragmentPath) {
	TextShader(LoadRawShader(vertexPath, fragmentPath));
}

TextShader::~TextShader() {
	glDeleteProgram(shaderID);
}
void TextShader::SetView(glm::mat4 view) {
	SetShaderMat4(unifview, view);
}

void TextShader::SetColor(float r, float g, float b, float a) {
	glUniform4f(unifColor, r, g, b, a);
}