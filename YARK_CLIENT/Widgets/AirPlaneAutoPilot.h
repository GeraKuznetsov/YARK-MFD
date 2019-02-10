#pragma once
#include "Control.h"

class AirPlaneAutoPilot : public Control {
	bool enabled;

	int16_t THROTTLE;

	uint32_t lastID = 0;
	double lastSpead = 0;
	double lastTime = 0;
	//void FloatPM(Draw* draw, float* f, int dig);
public:
	AirPlaneAutoPilot(WidgetStuff ws);
	void Tick(Draw* draw);
};