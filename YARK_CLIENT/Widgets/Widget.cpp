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

void Widget::WindowUpdate(Draw* draw) {
	draw->BindDraw2DShader();
	draw->BindTex2D(0);
	draw->SetDrawColor2D(0, 0, 1);

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
		}
		else {
			drag = false;
		}
	}
	draw->DrawRect2D(pos.x - BORDER, pos.y - BAR_HEIGHT, pos.x + size.x + BORDER, pos.y + size.y + BORDER);


	draw->BindTextShader();
	draw->SetTextColor(1, 1, 1);
	draw->DrawString(f, title, pos.x, pos.y);

	glScissor(pos.x, win->size.y - pos.y - size.y, size.x, size.y);
}