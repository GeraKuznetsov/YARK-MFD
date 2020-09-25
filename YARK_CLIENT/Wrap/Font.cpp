#pragma once

#include "Font.h"
#include <ft2build.h>
#include <iostream>
#include FT_FREETYPE_H  

Font::Font(int width, int height, const char* fontName) {
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	}

	FT_Face face;
	if (FT_New_Face(ft, fontName, 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	//return;
	FT_Set_Pixel_Sizes(face, width, height);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	size = width > height ? width : height;
	unsigned char* data = (unsigned char*)malloc(size * size);

	glGenTextures(1, &atlas);
	glBindTexture(GL_TEXTURE_2D_ARRAY, atlas);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_LINEAR_MIPMAP_LINEAR
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //GL_LINEAR

	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RED, size, size, 128, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	for (int c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}

		memset(data, 0, size * size);

		for (int y = 0; y < face->glyph->bitmap.rows; y++) {
			for (int x = 0; x < face->glyph->bitmap.width; x++) {
				data[x + y * size] = face->glyph->bitmap.buffer[x + face->glyph->bitmap.width * y];
			}
		}

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, c, size, size, 1, GL_RED, GL_UNSIGNED_BYTE, data);

		chars[c] = {
			(int)(face->glyph->bitmap_left),
			(int)(face->glyph->bitmap_top),
			(int)(face->glyph->advance.x)
		};
	}
	free(data);
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

Font::~Font() {
	glDeleteTextures(1, &atlas);
}

int Font::GetTextWidth(std::string text, int length, GLfloat scale) {
	int out = 0;
	if (length == -1) {
		length = text.size();
	}
	for (int i = 0; i < length; i++) {
		out += (chars[text[i]].Advance >> 6);
	}
	return out * scale;
}