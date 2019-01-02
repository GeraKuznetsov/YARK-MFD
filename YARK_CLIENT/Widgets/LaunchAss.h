#pragma once
#include "Widget.h"

class LaunchAss : public Widget {
public:
	LaunchAss(WidgetStuff ws);
	void Tick(Draw* draw);
};