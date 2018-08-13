#include "TextureLoader.h"
#include "../../Engine/Texture.h"
#include <iostream>

GLuint TextureLoader::getPlanetTexture(int SOI) {
	if (planetTextureCache[SOI]) {
		return planetTextureCache[SOI];
	}
	std::cout << "loading new : "+ std::to_string(SOI);
	GLuint texture = loadTexture("Tex/map/" + SOIs[SOI] + ".png");
	planetTextureCache[SOI] = texture;
	return texture;
}