#pragma once

#include "Widget.h"
#include "../Client/Client.h"
#include "../Wrap/Texture.h"
#include "../Wrap/Window.h"
#include "../Wrap/Draw.h"

class AtitudeIndicator : public Widget {
	GLuint chevronTex, triTex, tri_90;
public:
	std::string GetTitle();
	AtitudeIndicator();
	void Draw(XY pos, XY size);
};
