#pragma once
#include "../Engine/Window.h"
#include "../Client/Client.h"
#include "Util\TextureLoader.h"

struct WidgetStuff {
	XY pos; XY size; std::string title; Font* f; Window* win; Client** client; TextureLoader *TL;  std::string startUpName;
};

class Widget {
private:
	bool drag;
	XY windowDragStart;
	XY windowPosDragStart;
protected:
	XY pos;
	XY size;
	Font* f;
	Window* win;
	Client** client;
	bool mouseInWindow;
	TextureLoader *TL;
	std::string startUpName;
public:
	std::string title;
	Widget(WidgetStuff ws);
	void Resize(XY pos, XY size);
	virtual void Tick(Draw* draw);
	void WindowUpdate(Draw* draw);
	std::string getSaveParams();
};