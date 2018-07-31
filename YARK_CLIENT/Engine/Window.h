#pragma once
#include "glm.hpp"
#define GLEW_STATIC
#include <glew.h>
#define SDL_MAIN_HANDLED 
#include <SDL.h>
#include <SDL_opengl.h>
#include "Texture.h"
#include "Font.h"
#include "Draw.h"

#define NUMBER_KEYS 1024
#define NUMER_MOUSE_BUTTONS SDL_BUTTON_X2

#define FORCE_RES 1
#define STOP_RESIZE 4
#define FULL_SCREEN (2 | FORCE_RES)

typedef glm::ivec2 XYi;

class Window {
	bool keyRepeating[NUMBER_KEYS];
	bool keyDown[NUMBER_KEYS];
	bool keyTyped[NUMBER_KEYS];
	bool mouseDown[NUMER_MOUSE_BUTTONS];
	bool mouseClicked[NUMER_MOUSE_BUTTONS];
	bool mouseDoubleClicked[NUMER_MOUSE_BUTTONS];

	XYi mouseXY, mouseDXY;

	XYi MouseScroll;

	bool running = false;
	SDL_GLContext gGlContext;
	int targetRate;
	Draw *draw;
public:

	XYi size;
	SDL_Window* gWindow = NULL; //move back

	bool MouseInWindow = true;
	bool HasFocus = false;
	int FPS = 0; //theoFPS only works when vsync is on, otherwise it should be around real FPS
	void SetTargetFPS(int fps);
	Window(XYi s, int flags, int *error);
	void Run(void(*tick)(float delta, Draw* draw));
	bool KeyDown(int key);
	bool KeyRepeating(int key);
	bool KeyTyped(int key);
	bool MouseDown(int button);
	bool MouseClicked(int button);
	bool MouseDoubleClicked(int button);
	//void SetShowCursor(bool b);
	void SetSize(XYi size);
	int MouseX(), MouseY(), MouseDX(), MouseDY();
	XYi MouseXY(), MouseDXY();
	XYi getMouseWheelDelta();
	XYi getSize();
};
