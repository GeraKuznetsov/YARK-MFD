#include "Console.h"

void command(std::string);

Console::Console(Window* win, XY pos) :Widget(XY{ 0,0 }, XY{ 0,0 }, "qyefbj",0) {
	w = win;
	f = new Font(16, 16, "C:\\Windows\\Fonts\\lucon.ttf");
	this->pos = pos;
	this->size = size;
	for (int y = 0; y < CONSOLE_HEIGHT; y++) {
		for (int x = 0; x < CONSOLE_WIDTH; x++) {
			data[y*CONSOLE_WIDTH + x] = ' ';
			color[y*CONSOLE_WIDTH + x] = 0;
		}
	}
	memset(type, 0, CONSOLE_WIDTH);
	colorCur = 0;
	curPos = 0;
}
void Console::Tick(Draw* draw) {
/*	if (curPos < CONSOLE_WIDTH) {
		for (int i = SDL_SCANCODE_A; i <= SDL_SCANCODE_Z; i++) {
			if (w->KeyRepeating(i)) {
				type[curPos] = (i - SDL_SCANCODE_A + 'A');
				curPos++;
			}
		}
		for (int i = SDL_SCANCODE_0; i <= SDL_SCANCODE_9; i++) {
			if (w->KeyRepeating(i)) {
				type[curPos] = (i - SDL_SCANCODE_0 + '0');
				curPos++;
			}
		}
		if (w->KeyRepeating(SDL_SCANCODE_SPACE)) {
			type[curPos] = ' ';
			curPos++;
		}
	}
	if (curPos != 0) {
		if (w->KeyRepeating(SDL_SCANCODE_BACKSPACE)) {
			curPos--;
			type[curPos] = 0;
		}
		if (w->KeyTyped(SDL_SCANCODE_RETURN)) {
			command(std::string(type));
			memset(type, 0, CONSOLE_WIDTH);
			curPos = 0;
		}
	}*/

	draw->BindDraw2DShader();
	draw->BindTex2D();
	draw->SetDrawColor2D(0, 0, 1);
	draw->DrawRect2D(pos.x, pos.y, pos.x + size.x, pos.y + size.y);

	draw->BindTextShader();
	for (int y = 0; y < CONSOLE_HEIGHT; y++) {
		for (int x = 0; x < CONSOLE_WIDTH; x++) {
			char col = color[y*CONSOLE_WIDTH + x];
			if (col == 0) {
				draw->SetTextColor(1, 1, 1);
			}
			else if (col == 1) {
				draw->SetTextColor(1, 0, 0);
			}
			else if (col == 2) {
				draw->SetTextColor(0, 1, 0);
			}
			draw->DrawChar(f, data[y*CONSOLE_WIDTH + x], pos.x + x * CONSOLE_FONT_SIZE, pos.y + y * CONSOLE_FONT_SIZE);
		}
	}

	draw->SetTextColor(1, 1, 1);
	int x;
	for (x = 0; x < CONSOLE_WIDTH; x++) {
		if (type[x] == 0) {
			break;
		}
		draw->DrawChar(f, type[x], pos.x + x * CONSOLE_FONT_SIZE, pos.y + CONSOLE_HEIGHT * CONSOLE_FONT_SIZE);
	}
	draw->DrawChar(f, '_', pos.x + x * CONSOLE_FONT_SIZE, pos.y + CONSOLE_HEIGHT * CONSOLE_FONT_SIZE);
}
void Console::DispLine(std::string text) {
	memcpy(data, data + CONSOLE_WIDTH, CONSOLE_HEIGHT*CONSOLE_WIDTH - CONSOLE_WIDTH);
	memset(data + CONSOLE_HEIGHT * CONSOLE_WIDTH - CONSOLE_WIDTH, ' ', CONSOLE_WIDTH);
	memcpy(data + CONSOLE_HEIGHT * CONSOLE_WIDTH - CONSOLE_WIDTH, text.data(), text.length());

	memcpy(color, color + CONSOLE_WIDTH, CONSOLE_HEIGHT*CONSOLE_WIDTH - CONSOLE_WIDTH);
	memset(color + CONSOLE_HEIGHT * CONSOLE_WIDTH - CONSOLE_WIDTH, 0, CONSOLE_WIDTH);
	memset(color + CONSOLE_HEIGHT * CONSOLE_WIDTH - CONSOLE_WIDTH, colorCur, text.length());
}