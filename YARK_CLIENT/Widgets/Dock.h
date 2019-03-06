#pragma once
#include "NavBall.h"

class Dock : public NavBall {
public:
	Dock(WidgetStuff ws);
	void Tick(Draw* draw);
	void LoadNavBallTextures();
};