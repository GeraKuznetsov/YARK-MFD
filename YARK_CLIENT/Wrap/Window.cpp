#include "Window.h"
#include <iostream>
#include <string>
#include <chrono>

#define VSYNC 1
#define sout(x) std::cout << x << "\n"

Window::Window(XY s, bool fullScreen, int* error) {
	size = s;
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "SDL cannot init with error " << SDL_GetError() << std::endl;
		*error = 1;
		return;
	}
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		*error = 1;
		return;
	}
	if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0)
	{
		printf("SDL could not initialize joystick! SDL Error: %s\n", SDL_GetError());
		//*error = 1;
		//return;
	}

	//Check for joysticks
	if (SDL_NumJoysticks() < 1)
	{
		printf("Warning: No joysticks connected!\n");
	}
	else
	{
		//Load joystick
		gGameController = SDL_JoystickOpen(0);
		printf("joystick connected\n");
		if (gGameController == NULL)
		{
			printf("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError());
		}
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	if (fullScreen) {
		sout("test force res");
		SDL_DisplayMode mode;
		for (int i = 1; i < SDL_GetNumDisplayModes(0); i++) {
			SDL_GetDisplayMode(0, i, &mode);
			if (mode.h > size.y && mode.w > size.x) {
				size.x = mode.w;
				size.y = mode.h;
				break;
			}
		}
	}
	if ((gWindow = SDL_CreateWindow("Opengl", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, size.x, size.y, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN)) == NULL)
	{
		std::cout << "Cannot create window: " << SDL_GetError() << std::endl;
		*error = 1;
		return;
	}
	SDL_SetWindowResizable(gWindow, SDL_TRUE);
	if (fullScreen) {
		SDL_SetWindowFullscreen(gWindow, SDL_WINDOW_FULLSCREEN);
	}
	if ((gGlContext = SDL_GL_CreateContext(gWindow)) == NULL)
	{
		std::cout << "openGL context error: " << SDL_GetError() << std::endl;
		*error = 1;
		return;
	}

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		*error = 1;
		return;
	}

	for (int i = 0; i < NUMBER_KEYS; i++) {
		keyRepeating[i] = false;
		keyDown[i] = false;
		keyTyped[i] = false;
	}

	for (int i = 0; i < NUMER_MOUSE_BUTTONS; i++) {
		mouseDown[i] = false;
		mouseClicked[i] = false;
		mouseDoubleClicked[i] = false;
	}


	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, size.x, size.y);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	MouseScroll = mouseXY = mouseDXY = XY{ 0,0 };
	targetRate = 0;
	*error = 0;
}

void Window::Run(void(*tick)(float delta)) {
	running = true;
	SDL_Event sdlEvent;
	int fpsCounter = 0;
	Uint32  timeCounter = 0, time, lastFrameTime, delta;
	lastFrameTime = SDL_GetTicks();
	SDL_GL_SetSwapInterval(VSYNC);
	while (running) {
		time = SDL_GetTicks();
		delta = time - lastFrameTime;
		lastFrameTime = time;
		fpsCounter++;
		timeCounter += delta;
		if (timeCounter > 1000.f) {
			FPS = fpsCounter;
			fpsCounter = 0;
			timeCounter -= 1000;
		}
		if (targetRate) {
			if (delta < targetRate) {
				SDL_Delay(targetRate - delta);
			}
		}
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		while (SDL_PollEvent(&sdlEvent)) {
			switch (sdlEvent.type) {
			case  SDL_QUIT:
				if (onExit) {
					running = onExit();
				}
				else {
					running = false;
				}
				break;
			case SDL_KEYUP:
				//	keys[sdlEvent.key.] = true;
				keyDown[sdlEvent.key.keysym.scancode] = false;
				keyRepeating[sdlEvent.key.keysym.scancode] = false;
				keyTyped[sdlEvent.key.keysym.scancode] = true;
				break;
			case	SDL_KEYDOWN:
				keyDown[sdlEvent.key.keysym.scancode] = true;
				keyRepeating[sdlEvent.key.keysym.scancode] = true;
				break;
			case SDL_MOUSEBUTTONUP:
				mouseDown[sdlEvent.button.button] = false;
				mouseClicked[sdlEvent.button.button] = true;
				if (sdlEvent.button.clicks == 2) {
					mouseDoubleClicked[sdlEvent.button.button] = true;
				}
				break;
			case SDL_MOUSEMOTION:
				//mouseDXY = (mouseDXY - MouseXY)  + &XYi{ sdlEvent.motion.x, sdlEvent.motion.y } ;
				//mouseDXY = mouseDXY + XYi{ sdlEvent.motion.x, sdlEvent.motion.y } -mouseXY;
				mouseDXY = XY{ sdlEvent.motion.xrel,sdlEvent.motion.yrel };
				mouseXY = XY{ sdlEvent.motion.x, sdlEvent.motion.y };
				break;
			case SDL_MOUSEBUTTONDOWN:
				mouseDown[sdlEvent.button.button] = true;
				break;
			case SDL_MOUSEWHEEL:
				MouseScroll = MouseScroll + XY{ sdlEvent.wheel.x ,sdlEvent.wheel.y };
				break;
			case SDL_JOYAXISMOTION:
				//std::cout << "axis: " << std::to_string(sdlEvent.jaxis.axis) << " value: " << std::to_string(sdlEvent.jaxis.value) << "\n";
				for (int i = 0; i < 4; i++) {
					if (sdlEvent.jaxis.axis == i) {
						joystickDir[i] = sdlEvent.jaxis.value;
					}
				}
			case SDL_WINDOWEVENT:
				switch (sdlEvent.window.event) {
				case SDL_WINDOWEVENT_SHOWN:
					HasFocus = true;
					break;
				case SDL_WINDOWEVENT_HIDDEN:
					HasFocus = false;
					break;
				case SDL_WINDOWEVENT_EXPOSED:
					HasFocus = true;
					break;
				case SDL_WINDOWEVENT_MINIMIZED:
					HasFocus = false;
					break;
				case SDL_WINDOWEVENT_MAXIMIZED:
					break;
				case SDL_WINDOWEVENT_RESTORED:
					HasFocus = true;
					break;
				case SDL_WINDOWEVENT_ENTER:
					MouseInWindow = true;
					break;
				case SDL_WINDOWEVENT_LEAVE:
					MouseInWindow = false;
					break;
				case SDL_WINDOWEVENT_CLOSE:
					running = false;
					break;
				case SDL_WINDOWEVENT_FOCUS_LOST:
					HasFocus = false;
					break;
				case SDL_WINDOWEVENT_TAKE_FOCUS:
					SDL_Log("Window %d is offered a focus", sdlEvent.window.windowID);
					break;
				case SDL_WINDOWEVENT_HIT_TEST:
					SDL_Log("Window %d has a special hit test", sdlEvent.window.windowID);
					break;
				}
			}
		}
		SDL_GetWindowSize(gWindow, &size.x, &size.y);
		glViewport(0, 0, size.x, size.y);

		tick(float(delta));

		mouseDXY = XY{ 0,0 }; //reset delta events
		for (int i = 0; i < NUMER_MOUSE_BUTTONS; i++) {
			mouseClicked[i] = false;
			mouseDoubleClicked[i] = false;
		}
		for (int i = 0; i < NUMBER_KEYS; i++) {
			keyTyped[i] = false;
		}
		SDL_GL_SwapWindow(gWindow);
	}
	SDL_DestroyWindow(gWindow);
	SDL_JoystickClose(gGameController);
	gWindow = NULL;

	SDL_Quit();
}

void Window::SetTargetFPS(int fps) {
	if (fps) {
		targetRate = 2000.f / fps; //why the fuck is it 2000 ??!!!!??? (and not 1000)
	}
	else {
		targetRate = 0;
	}
}

bool Window::KeyDown(int key) {
	return keyDown[key];
}
bool Window::KeyRepeating(int key) {
	bool b = keyRepeating[key];
	keyRepeating[key] = 0;
	return b;
}
bool Window::KeyTyped(int key) {
	bool b = keyTyped[key];
	return b;
}
bool Window::MouseDown(int button) {
	return mouseDown[button];
}
bool Window::MouseClicked(int button) {
	return mouseClicked[button];
}
bool Window::MouseDoubleClicked(int button) {
	return mouseDoubleClicked[button];
}
/*
void Window::SetShowCursor(bool b) {
	SDL_ShowCursor(SDL_DISABLE);
}*/
void Window::SetSize(XY size) {
	SDL_SetWindowSize(gWindow, size.x, size.y);
}
int Window::MouseX()
{
	return mouseXY.x;
}
int Window::MouseY()
{
	return mouseXY.y;
}
int Window::MouseDX()
{
	return mouseDXY.x;
}
int Window::MouseDY()
{
	return mouseDXY.y;
}
XY Window::MouseXY()
{
	return mouseXY;
}
XY Window::MouseDXY()
{
	return mouseDXY;
}

XY Window::getMouseWheelDelta() {
	XY ret = MouseScroll;
	MouseScroll = XY{ 0,0 };
	return ret;
}
XY  Window::getSize() {
	return  size;
}
bool Window::HasJoyStick() {
	return !!gGameController;
}