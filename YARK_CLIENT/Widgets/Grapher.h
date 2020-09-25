#pragma once
#include "Widget.h"
class Grapher : public Widget {
	int statI;
public:
	std::string GetTitle();
	Grapher(int i);
	void Draw(XY pos, XY size);
};
