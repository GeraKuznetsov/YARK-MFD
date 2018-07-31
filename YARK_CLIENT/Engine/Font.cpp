#pragma once

#include "Font.h"
#include <ft2build.h>
#include <iostream>
#include FT_FREETYPE_H  

Font::Font(int width, int height, const char * fontName) {
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	}

	FT_Face face;
	if (FT_New_Face(ft, fontName, 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	//return;
	FT_Set_Pixel_Sizes(face, width, height);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		GLuint texture;
		glGenTextures(1, &texture);

		int tWidth = 1;
		while (tWidth < face->glyph->bitmap.width)
			tWidth *= 2;
		int tHeight = 1;
		while (tHeight < face->glyph->bitmap.rows)
			tHeight *= 2;

		GLubyte* expanded_data = new GLubyte[tWidth * tHeight];
		for (int y = 0; y < tHeight; y++) {
			for (int x = 0; x < tWidth; x++) {
				expanded_data[(x + y * tWidth)] = (x >= face->glyph->bitmap.width || y >= face->glyph->bitmap.rows) ? 0 : face->glyph->bitmap.buffer[x + face->glyph->bitmap.width*y];
			}
		}


		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, tWidth, tHeight, 0, GL_RED, GL_UNSIGNED_BYTE, expanded_data);

		delete[] expanded_data;

		Character character = {
			texture,
			tWidth,
			tHeight,
			face->glyph->bitmap_left,
			face->glyph->bitmap_top,
			face->glyph->advance.x
		};
		font.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

Font::~Font() {
	for (int i = 0; i < font.size(); i++) {
		glDeleteTextures(1, &font[i].TextureID);
	}
}

int Font::GetTextWidth(std::string text, int length, GLfloat scale) {
	int out = 0;
	if (length == -1) {
		length = text.size();
	}
	for (int i = 0; i < length; i++) {
		out += (font[text[i]].Advance >> 6);
	}
	return out * scale;
}