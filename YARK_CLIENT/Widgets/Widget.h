#pragma once
#include "../Wrap/Window.h"
#include "../Wrap/Draw.h"
#include "Util/IM.h"
#include "Util/TextureLoader.h"
#include "../Client/Client.h"

extern Window* win;
extern Font* f;
extern Draw* draw;
extern Client client;
extern TextureLoader TL;

#define TEXT_OFFSET 20

class Widget {
public:
	Widget();
	virtual std::string GetTitle();
	virtual void Draw(XY pos, XY size);
};

