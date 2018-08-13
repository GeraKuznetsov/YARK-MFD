#include "Widget.h"

#define BAR_HEIGHT 15
#define BORDER 4

#include <iostream>

Widget::Widget(WidgetStuff ws) {
	Resize(ws.pos, ws.size);
	this->f = ws.f;
	this->title = ws.title;
	this->win = ws.win;
	this->client = ws.client;
	this->TL = ws.TL;
	this->startUpName = ws.startUpName;
}
void Widget::Resize(XY pos, XY size) {
	this->pos = pos;
	this->size = size;
	this->pos.y += BAR_HEIGHT;
	this->size.y -= BAR_HEIGHT + BORDER;
	this->pos.x += BORDER;
	this->size.x -= BORDER * 2;
}

void Widget::Tick(Draw* draw) {
	WindowUpdate(draw);
}

std::string Widget::getSaveParams() {
	return startUpName + " " + std::to_string(pos.x - BORDER) + " " + std::to_string(pos.y - BAR_HEIGHT) + " " + std::to_string(size.x + BORDER * 2) + " " + std::to_string(size.y + BAR_HEIGHT + BORDER) + "\n";
}

extern std::vector<Widget*> widgets;

void Widget::WindowUpdate(Draw* draw) {
	draw->BindDraw2DShader();
	draw->BindTex2D(0);
	draw->SetDrawColor2D(0, 0, 1);

	bool moveTofront = false;

	mouseInWindow = (win->MouseX() > pos.x && win->MouseY() > pos.y && win->MouseX() < pos.x + size.x && win->MouseY() < pos.y + size.y);
	if (drag || (win->MouseX() > pos.x - BORDER && win->MouseY() > pos.y - BAR_HEIGHT && win->MouseX() < pos.x + size.x + BORDER && win->MouseY() < pos.y)) {
		draw->SetDrawColor2D(0.2, 0, 0.8);
		if (win->MouseDown(SDL_BUTTON_LEFT)) {
			if (!drag) {
				windowPosDragStart = pos;
				windowPosDragStart.y -= BAR_HEIGHT;
				windowDragStart = win->MouseXY();
				drag = true;
			}
			pos.x = ((windowPosDragStart.x - windowDragStart.x + win->MouseX()) / 10) * 10 + BORDER;
			pos.y = ((windowPosDragStart.y - windowDragStart.y + win->MouseY()) / 10) * 10 + BAR_HEIGHT;
			std::cout << win->MouseX() << " : " << windowDragStart.x << "\n";
			//posDragStart.x - dragStart.x + win->MouseX() - BORDER, posDragStart.y - dragStart.y + win->MouseY() - BAR_HEIGHT
			moveTofront = true;
		}
		else {
			drag = false;
		}
	}

	if (((win->MouseDown(SDL_BUTTON_LEFT) || win->MouseDown(SDL_BUTTON_MIDDLE) || win->MouseDown(SDL_BUTTON_RIGHT)) && mouseInWindow) || moveTofront) {
		int index = 0;
		for (int i = 0; i < widgets.size(); i++) {
			if (widgets[i] == this) {
				index = i;
				break;
			}
		}
		widgets.push_back(widgets[index]);
		widgets.erase(widgets.begin() + index);
	}


	draw->DrawRect2D(pos.x - BORDER, pos.y - BAR_HEIGHT, pos.x + size.x + BORDER, pos.y + size.y + BORDER);


	draw->BindTextShader();
	draw->SetTextColor(1, 1, 1);
	draw->DrawString(f, title, pos.x, pos.y);

	glScissor(pos.x, win->size.y - pos.y - size.y, size.x, size.y);

	draw->BindDraw2DShader();
	draw->SetDrawColor2D(0, 0, 0);
	draw->DrawRect2D(pos.x, pos.y, pos.x + size.x, pos.y + size.y);
}