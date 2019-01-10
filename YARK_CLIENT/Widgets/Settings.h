#pragma once
#include "Widget.h"

class Settings: public Widget {
	void RadioOpt(std::string option, std::string key, Draw* draw);
	XY lastPos;
public:
	Settings(WidgetStuff ws);
	void Tick(Draw* draw);
};