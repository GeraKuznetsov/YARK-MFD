#pragma once
#include "../Engine/Draw.h"

struct XY {
	int x, y;
};

class Widget {
protected:
	XY pos;
	XY size;
	Font* f;

public:
	std::string title;
	Widget(XY pos, XY size, std::string title, Font* f);
	void Resize(XY pos, XY size);
	virtual void Tick(Draw* draw);
	void RenderWindow(Draw* draw);
};