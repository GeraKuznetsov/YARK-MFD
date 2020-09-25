#pragma once
#include <string>
#include <vector>

struct BodyInfo {
	std::string name;
	float rad;
	float soiDist;
	float mass;
};

#define NUM_BODIES 17

extern BodyInfo Bodies[NUM_BODIES];
