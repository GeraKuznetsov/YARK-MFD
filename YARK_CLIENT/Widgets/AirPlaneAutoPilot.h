#pragma once
#include "Control.h"

class AirPlaneAutoPilot : public Widget {
	bool maintainSpeed =0 , maintainAlt=0;
	float targetAlt = 2000, targetSpeed = 200, maxPitch = 10, maxAoA = 5;
	int open = 0;
	uint32_t lastID = 0;
	double lastSpead = 0;
	double lastTime = 0;
public:
	std::string GetTitle();
	AirPlaneAutoPilot();
	void Draw(XY pos, XY size);
};