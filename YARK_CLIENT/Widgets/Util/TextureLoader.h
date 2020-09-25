#pragma once
#include <string>
#include "../../Wrap/Draw.h"
#include "BodyInfo.h"
#include "../../Client/Structs.h"

#define BIG_TEX_NUM_X 2
#define BIG_TEX_NUM_Y 1

struct PlanetTexCache {
	GLuint ID;
};

class TextureLoader {
	GLuint sasModeTexs[SAS_HOLD_VECTOR + 4];
	PlanetTexCache planetTexHigh[NUM_BODIES];
	PlanetTexCache planetTexLow[NUM_BODIES];
public:
	void Start();
	GLuint getPlanetTextureHigh(int SOI);
	GLuint getPlanetTextureLow(int SOI);
	GLuint SASModeTex(int SASMode);
};