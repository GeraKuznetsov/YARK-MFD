#pragma once
#include "NavBall.h"

class SoyuzNavBall :public NavBall {
	int lastSOI = -1;
	GLuint textureID;
public:
	SoyuzNavBall(WidgetStuff ws);
	void LoadNavBallTextures();
	void Tick(Draw* draw);
};