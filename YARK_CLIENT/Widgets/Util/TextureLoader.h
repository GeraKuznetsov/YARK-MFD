#pragma once
#include <string>
#include "../../Engine/Draw.h"
#define NUM_PLANETS 17

const std::string SOIs[NUM_PLANETS] = { "sun", "moho", "eve", "gilly","kerbin","mun","minmus","duna","ike","dres","jool","laythe","vall","tylo","bop","pol","eeloo" };

class TextureLoader {
	GLuint planetTextureCache[NUM_PLANETS];
public:
	GLuint getPlanetTexture(int SOI);
};