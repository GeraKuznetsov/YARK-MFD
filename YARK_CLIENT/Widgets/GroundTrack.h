#pragma once
#include "Widget.h"
#include "Util/OrbitCalc.h"

//stollen from KSP itself
class GroundTrack : public Widget {
	GLuint gtVBO = 0, gtVAO = 0;
	int VBOsize = 0;
	void DrawOrbit(XY pos, XY size, OrbitData* orbit, float startTime, float startMA, glm::vec3 color, int periods, bool dots);
public:
	std::string GetTitle();
	GroundTrack();
	void Draw(XY pos, XY size);
};