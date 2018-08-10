#pragma once

#include "Widget.h"
#include "../Client/Client.h"
#include "../Engine/Texture.h"
#include "../Engine/Window.h"
#include "../Engine/Draw.h"

class AtitudeIndicator : public Widget {
	GLuint chevronTex, triTex, tri_90;
public:
	AtitudeIndicator(WidgetStuff ws);
	void Tick(Draw* draw);
};
