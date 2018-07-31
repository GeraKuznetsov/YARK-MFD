#pragma once
#define GLEW_STATIC
#include <glew.h>
#include <map>

struct Character {
	GLuint TextureID;   // ID handle of the glyph texture
	int sizeX;    // Size of glyph
	int sizeY;
	int bearingX;  // Offset from baseline to left/top of glyph
	int bearingY;
	GLuint Advance;    // Horizontal offset to advance to next glyph
};

class Font {
public:
	std::map<GLchar, Character> font;
	Font(int width, int height, const char * fontName);
	~Font();
	int GetTextWidth(std::string text, int length = -1, GLfloat scale = 1);
};