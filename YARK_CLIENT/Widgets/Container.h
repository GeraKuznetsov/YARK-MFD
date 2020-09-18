#pragma once
#include "Widget.h"
#include "../Engine/Window.h"
#include "../Engine/Draw.h"
#include "NavBall.h"
#include "AirMap.h"
#include "AtitudeIndicator.h"
#include "SoyuzNavBall.h"
#include "Orbit.h"
#include "Control.h"
#include "AirPlaneAutoPilot.h"

extern Widget* optionsWidget;
extern NavBall* navballWidget;
extern AirMap* airMapWidget;
extern AtitudeIndicator* atiinWidget;
extern SoyuzNavBall* soyuzNavBallWidget;
extern OrbitDisplay* orbitDisplayWidget;
extern Control* controlWidget;
extern AirPlaneAutoPilot* airPlaneAutoPilotWidget;

#define WID_BORDER 4
#define BAR_HEIGHT 20
extern Window* win;
extern Font* f;
extern Draw* draw;

extern GLuint window_x, window_lr, window_ud;
void ContainerLoadWidgets();
void LoadWindowIcons();

enum SPLIT {
	NONE,
	LEFT_RIGHT,
	UP_DOWN,
};

struct Container {
	Widget* widget;
	bool drag = false;
	Container(Container* l, Container* r);
	Container(Widget* w);
	Container *left, *right;
	SPLIT split = NONE;
	float division;
	//int posX, posY, width, height;
	bool Update(bool top, int posX, int posY, int width, int height);
};