#pragma once
#include "../Engine/Draw.h"

struct XY {
	int x, y;
};

class Widget {
protected:
	XY pos;
	XY size;
	std::string title;
	Font* f;

public:
	Widget(XY pos,XY size,std::string title,Font* f);
	void Tick(Draw* draw);
	void RenderWindow(Draw* draw);
};