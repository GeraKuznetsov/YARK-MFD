#pragma once
#define GLEW_STATIC
#include <glew.h>
#include <map>
#include <string>

struct Character {
	int bearingX;  // Offset from baseline to left/top of glyph
	int bearingY;
	int Advance;    // Horizontal offset to advance to next glyph
};

class Font {
public:
	int size;
	Character chars[128];
	GLuint atlas;
	Font(int width, int height, const char* fontName);
	~Font();
	int GetTextWidth(std::string text, int length = -1, GLfloat scale = 1);
};