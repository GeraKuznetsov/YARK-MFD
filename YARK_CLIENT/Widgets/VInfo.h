#pragma once
#include "Widget.h"

class VInfo : public Widget {
public:
	VInfo(WidgetStuff ws);
	void Tick(Draw* draw);
};