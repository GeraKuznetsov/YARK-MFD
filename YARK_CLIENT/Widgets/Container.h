#pragma once
#include "Widget.h"
#include "../Wrap/Window.h"
#include "../Wrap/Draw.h"
#include "NavBall.h"
#include "AirMap.h"
#include "AtitudeIndicator.h"
#include "SoyuzNavBall.h"
#include "Orbit.h"
#include "Control.h"
#include "AirPlaneAutoPilot.h"
#include "Dock.h"

//extern Widget* optionsWidget;
//extern NavBall* navballWidget;
//extern AirMap* airMapWidget;
//extern AtitudeIndicator* atiinWidget;
//extern SoyuzNavBall* soyuzNavBallWidget;
//extern OrbitDisplay* orbitDisplayWidget;
//extern Control* controlWidget;
//extern AirPlaneAutoPilot* airPlaneAutoPilotWidget;
//extern Dock* dockingWidget;

#define WID_BORDER 4
#define BAR_HEIGHT 20
extern Window* win;
extern Font* f;
extern Draw* draw;
#include <vector>

extern GLuint window_x, window_lr, window_ud, swap_h, swap_v;
void ContainerLoadWidgets();
void LoadWindowIcons();

enum class SPLIT {
	NONE,
	LEFT_RIGHT,
	UP_DOWN,
};

struct Container {
	std::string Save();
	Widget* widget;
	bool drag = false;
	Container(Container* l, Container* r);
	Container(Widget* w);
	Container(std::string data);
	Container* left = 0, * right = 0;
	SPLIT split = SPLIT::NONE;
	float division;
	//int posX, posY, width, height;
	bool Update(bool top, int posX, int posY, int width, int height);
	void Free();
};

extern std::vector<Widget*> widgets;