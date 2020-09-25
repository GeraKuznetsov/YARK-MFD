#include "TextureLoader.h"
#include "../../Wrap/Texture.h"
#include <iostream>
#include "../lodepng/lodepng.h"

GLuint TextureLoader::getPlanetTextureHigh(int SOI) {
	if (planetTexHigh[SOI].ID) {
		return planetTexHigh[SOI].ID;
	}
	unsigned int width, height;
	std::vector<unsigned char> image;
	std::string filename = "Tex/PlanetHigh/"+ Bodies[SOI].name + ".png";
	printf("Loading %s\n", filename.c_str());
	int e;
	if (e = lodepng::decode(image, width, height, filename, LodePNGColorType::LCT_RGB)) {
		std::cout << "Could not load image: " + filename << "\n";
	}
	unsigned int tileWidth = width / BIG_TEX_NUM_X;
	unsigned int tileHeight = height / BIG_TEX_NUM_Y;

	glGenTextures(1, &planetTexHigh[SOI].ID);
	glBindTexture(GL_TEXTURE_2D_ARRAY, planetTexHigh[SOI].ID);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_LINEAR_MIPMAP_LINEAR
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //GL_LINEAR
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB, tileWidth, tileHeight, BIG_TEX_NUM_X * BIG_TEX_NUM_Y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	unsigned char* tile = (unsigned char*)malloc(tileWidth * tileHeight * 3);
	for (int x = 0; x < BIG_TEX_NUM_X; x++) {
		for (int y = 0; y < BIG_TEX_NUM_Y; y++) {
			for (int cY = 0; cY < tileHeight; cY++) {
				memcpy((void*)(tile + (cY * tileWidth) * 3), (const void*)(image.data() + ((cY + y * tileHeight) * width + (x * tileWidth)) * 3), tileWidth * 3);
			}
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, (y * BIG_TEX_NUM_X + x), tileWidth, tileHeight, 1, GL_RGB, GL_UNSIGNED_BYTE, tile);
		}
	}
	free(tile);
	image.clear();
	image.shrink_to_fit();
	return planetTexHigh[SOI].ID;
}

GLuint TextureLoader::getPlanetTextureLow(int SOI) {
	if (planetTexLow[SOI].ID) {
		return planetTexLow[SOI].ID;
	}
	std::cout << "loading new : " + std::to_string(SOI);
	GLuint texture = loadTexture("Tex/PlanetLow/" + Bodies[SOI].name + ".png");
	planetTexLow[SOI].ID = texture;
	return texture;
}

void TextureLoader::Start() {
	sasModeTexs[SAS_HOLD] = loadTexture("Tex/navball/HOLD.png");
	sasModeTexs[SAS_PROGRADE] = loadTexture("Tex/navball/PG.png");
	sasModeTexs[SAS_RETROGRADE] = loadTexture("Tex/navball/RG.png");
	sasModeTexs[SAS_RADOUT] = loadTexture("Tex/navball/RO.png");
	sasModeTexs[SAS_RADIN] = loadTexture("Tex/navball/RI.png");
	sasModeTexs[SAS_NORMAL] = loadTexture("Tex/navball/N.png");
	sasModeTexs[SAS_ANTINORMAL] = loadTexture("Tex/navball/AN.png");
	sasModeTexs[SAS_TARGET] = loadTexture("Tex/navball/TPG.png");
	sasModeTexs[SAS_ANTITARGET] = loadTexture("Tex/navball/TRG.png");
	sasModeTexs[SAS_MAN] = loadTexture("Tex/navball/M.png");
	sasModeTexs[SAS_HOLD_VECTOR] = loadTexture("Tex/navball/SASS.png");
	sasModeTexs[SAS_HOLD_VECTOR + 1] = loadTexture("Tex/navball/cross.png");
	sasModeTexs[SAS_HOLD_VECTOR + 2] = loadTexture("Tex/navball/dock.png");
	sasModeTexs[SAS_HOLD_VECTOR + 3] = loadTexture("Tex/navball/align.png");
}

GLuint TextureLoader::SASModeTex(int SASMode) {
	return sasModeTexs[SASMode];
}