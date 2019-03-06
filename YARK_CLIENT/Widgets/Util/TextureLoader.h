#pragma once
#include <string>
#include "../../Engine/Draw.h"
#define NUM_PLANETS 17

const std::string SOIs[NUM_PLANETS] = { "sun", "moho", "eve", "gilly","kerbin","mun","minmus","duna","ike","dres","jool","laythe","vall","tylo","bop","pol","eeloo" };

#include "../../Client/Structs.h"

class TextureLoader {
	GLuint planetTextureCache[NUM_PLANETS];
	//GLuint PGTex, RGTex, RITex, ROTex, NTex, ANTex, TRGTex, TPGTex, MTex, SASSTex;
	GLuint sasModeTexs[SAS_HOLD_VECTOR + 2];
	bool sasModesLoaded = false;
public:
	GLuint getPlanetTexture(int SOI);
	GLuint SASModeTex(int SASMode);
};