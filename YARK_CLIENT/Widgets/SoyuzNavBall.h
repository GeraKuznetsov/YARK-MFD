#pragma once
#include "NavBall.h"

class SoyuzNavBall :public NavBall {
	int lastSOI = -1;
	GLuint textureID;
public:
	std::string GetTitle();
	SoyuzNavBall();
	void LoadNavBallTextures();
	void Draw(XY pos, XY size);
};