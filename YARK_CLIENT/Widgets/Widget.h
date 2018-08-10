#pragma once
#include "../Engine/Window.h"
#include "../Client/Client.h"

struct XY {
	int x, y;
};

struct WidgetStuff {
	XY pos; XY size; std::string title; Font* f; Window* win; Client** client;
};

class Widget {
private: 
	bool drag;
	XYi windowDragStart; 
	XY windowPosDragStart;
protected:
	XY pos;
	XY size;
	Font* f;
	Window* win;
	Client** client;
	bool mouseInWindow;
public:
	std::string title;
	Widget(WidgetStuff ws);
	void Resize(XY pos, XY size);
	virtual void Tick(Draw* draw);
	void WindowUpdate(Draw* draw);
};