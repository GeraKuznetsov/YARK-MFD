#pragma once
#include "Control.h"

class AirPlaneAutoPilot : public Control {
	bool enabled;

	int16_t THROTTLE;

	uint32_t lastID = 0;
	double lastSpead = 0;
	double lastTime = 0;
public:
	std::string GetTitle();
	AirPlaneAutoPilot();
	void Draw(XY pos, XY size);
};