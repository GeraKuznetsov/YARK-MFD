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

GLuint TextureLoader::SASModeTex(int SASMode) {
	if (!sasModesLoaded) {
		sasModesLoaded = true;
		sasModeTexs[SAS_HOLD] = loadTexture("Tex/navball/HOLD.png");
		sasModeTexs[SAS_PROGRADE] =  loadTexture("Tex/navball/PG.png"); 
		sasModeTexs[SAS_RETROGRADE] =  loadTexture("Tex/navball/RG.png");
		sasModeTexs[SAS_RADIN] =  loadTexture("Tex/navball/RI.png");
		sasModeTexs[SAS_RADOUT] =  loadTexture("Tex/navball/RO.png");
		sasModeTexs[SAS_NORMAL] = loadTexture("Tex/navball/N.png");
		sasModeTexs[SAS_ANTINORMAL] =  loadTexture("Tex/navball/AN.png");
		sasModeTexs[SAS_TARGET] =  loadTexture("Tex/navball/TPG.png");
		sasModeTexs[SAS_ANTITARGET] =  loadTexture("Tex/navball/TRG.png");
		sasModeTexs[SAS_MAN] = loadTexture("Tex/navball/M.png");
		sasModeTexs[SAS_HOLD_VECTOR] =   loadTexture("Tex/navball/SASS.png");
	}
	return sasModeTexs[SASMode];
}