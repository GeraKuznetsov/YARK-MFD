#pragma once
#include "Widget.h"

class Settings: public Widget {
	void RadioOpt(std::string option, std::string key, XY pos, Draw* draw);
public:
	Settings(WidgetStuff ws);
	void Tick(Draw* draw);
};