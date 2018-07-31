#include "TextShader.h"
#include <iostream>

TextShader::TextShader(GLuint shader) {
	shaderID = shader;
	glUseProgram(shader);
	unifColor = glGetUniformLocation(shader, "color");
	unifOrthro = glGetUniformLocation(shader, "orthro");
	glUniform1i(glGetUniformLocation(shader, "tex"), 0);
}

TextShader::TextShader(const GLchar* vertexPath, const GLchar* fragmentPath) {
	TextShader(LoadRawShader(vertexPath, fragmentPath));
}

TextShader::~TextShader() {
	glDeleteProgram(shaderID);
}

void TextShader::SetColor(float r, float g, float b, float a) {
	glUniform4f(unifColor, r, g, b, a);
}