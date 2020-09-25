#pragma once
#include "NavBall.h"

class Dock : public Widget {
	bool killV = 0;
public:
	Dock();
	void Draw(XY pos, XY size);
	std::string GetTitle();
};