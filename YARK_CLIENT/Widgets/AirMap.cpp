#include "AirMap.h"

#define CROSS_WIDTH 1

AirMap::AirMap(WidgetStuff ws) :Widget(ws) {
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	longLatFixed = true;
	zoom = 50;

	kerbinMap = loadTexture("Tex/map/kerbin.png", false);
	Target tar;
	tar.start = wayPoint{ glm::vec2{ -74.726413 ,-0.0485981},67.f };
	tar.stop = wayPoint{ glm::vec2{ -74.490867 ,-0.050185}, 67 };
	tar.name = "KSC 09";
	tar.name2 = "KSC 21";
	targets.push_back(tar);
	tar.start = wayPoint{ glm::vec2{ -71.965488 ,-1.517306 },132.f };
	tar.stop = wayPoint{ glm::vec2{ -71.852408 ,-1.515980 }, 132.f };
	tar.name = "Island W";
	tar.name2 = "Island E";
	targets.push_back(tar);
	target = 0;
}

#define GLM_ENABLE_EXPERIMENTAL
#include "gtc\matrix_transform.hpp"
#include "gtx/rotate_vector.hpp"
#include <iostream>
#include "gtx/string_cast.hpp"

void AirMap::drawTarget(Target* t, Draw* draw, VesselPacket* VP, float zoom) {
	//longLat = glm::vec2(VP->Lon, VP->Lat);
	glm::vec2 start = glm::rotate(t->start.coord - longLat, glm::radians(VP->Heading));
	glm::vec2 stop = glm::rotate(t->stop.coord - longLat, glm::radians(VP->Heading));
	start *= zoom;
	stop *= zoom;
	float x1, y1, x2, y2, mx1, my1, mx2, my2;
	x1 = start.x + pos.x + size.x / 2;
	y1 = -start.y + pos.y + size.y / 2;
	x2 = stop.x + pos.x + size.x / 2;
	y2 = -stop.y + pos.y + size.y / 2;
	mx1 = min(x1, x2) - 5;
	mx2 = max(x1, x2) + 5;
	my1 = min(y1, y2) - 5;
	my2 = max(y1, y2) + 5;
	if (mouseInWindow && win->MouseX() > mx1 && win->MouseY() > my1 && win->MouseX() < mx2  && win->MouseY() < my2) {
		draw->SetDrawColor2D(1, 0, 0);
		if (win->MouseClicked(SDL_BUTTON_LEFT)) {
			if (target == t) {
				if (target->primary) {
					target->primary = false;
				}
				else {
					target = NULL;
				}
			}
			else {
				target = t;
				target->primary = true;
			}
		}
	}
	else {
		draw->SetDrawColor2D(1, 1, 1);
	}
	draw->DrawLine2D(x1, y1, x2, y2);

}

void AirMap::Tick(Draw* draw) {
	WindowUpdate(draw);

	VesselPacket VP = (*client) ? (*client)->vesselPacket : VesselPacket();

	if (mouseInWindow) {
		if (win->MouseDoubleClicked(SDL_BUTTON_MIDDLE)) {
			longLatFixed = true;
		}
		if (win->MouseDown(SDL_BUTTON_MIDDLE)) {
			if (!drag) {
				dragStart = win->MouseXY();
				longLatDragStart = longLat;
				longLatFixed = false;
				drag = true;
				head = VP.Heading;
			}
		}
		else {
			drag = false;
		}
		zoom += win->getMouseWheelDelta().y * zoom / 8;
		if (zoom < 2) {
			zoom = 2;
		}
	}
	if (drag) {
		VP.Heading = head;
		longLat = longLatDragStart + glm::rotate(glm::vec2(dragStart.x - win->MouseX(), win->MouseY() - dragStart.y), glm::radians(-head)) / zoom;
	}
	if (longLatFixed) {
		longLat = glm::vec2(VP.Lon, VP.Lat);
	}

	//float zoom = 0;
	glm::vec2 start, stop;
	if (target) {
		start = glm::rotate(target->start.coord - longLat, glm::radians(VP.Heading));
		stop = glm::rotate(target->stop.coord - longLat, glm::radians(VP.Heading));
		float dist = max(glm::length(start), glm::length(stop));
		if (longLatFixed) { zoom = (size.x / 2 - 20) / dist; }
	}
	glm::vec2 uv1, uv2, uv3, uv4, p1, p2, p3, p4; //ugly code to calulate map UVs
	glm::vec2 o1, o2, o3, o4;
	o1 = glm::rotate((p1 = glm::vec2(-size.x / 2, -size.y / 2)), glm::radians(-VP.Heading));
	o2 = glm::rotate((p2 = glm::vec2(size.x / 2, -size.y / 2)), glm::radians(-VP.Heading));
	o3 = glm::rotate((p3 = glm::vec2(size.x / 2, size.y / 2)), glm::radians(-VP.Heading));
	o4 = glm::rotate((p4 = glm::vec2(-size.x / 2, size.y / 2)), glm::radians(-VP.Heading));

	uv1 = longLat + o1 / zoom;
	uv2 = longLat + o2 / zoom;
	uv3 = longLat + o3 / zoom;
	uv4 = longLat + o4 / zoom;

	//std::cout << glm::to_string(longLat) << "\n";

	uv1 = uv1 / glm::vec2(360.f, 180.f) + glm::vec2(0.5f, 0.5f);
	uv2 = uv2 / glm::vec2(360.f, 180.f) + glm::vec2(0.5f, 0.5f);
	uv3 = uv3 / glm::vec2(360.f, 180.f) + glm::vec2(0.5f, 0.5f);
	uv4 = uv4 / glm::vec2(360.f, 180.f) + glm::vec2(0.5f, 0.5f);

	uv1.y = 1 - uv1.y;
	uv2.y = 1 - uv2.y;
	uv3.y = 1 - uv3.y;
	uv4.y = 1 - uv4.y;
	p1 *= glm::vec2(1.f, -1.f);
	p2 *= glm::vec2(1.f, -1.f);
	p3 *= glm::vec2(1.f, -1.f);
	p4 *= glm::vec2(1.f, -1.f);

	glm::vec2 cntr = glm::vec2(pos.x + size.x / 2, pos.y + size.y / 2);

	draw->BindDraw2DShader();
	draw->SetDrawColor2D(0, 0, 0);
	draw->DrawRect2D(pos.x, pos.y, pos.x + size.x, pos.y + size.y);
	draw->BindTex2D(kerbinMap);
	draw->SetDrawColor2D(1, 1, 1);
	draw->DrawRectUV2D(p1 + cntr, p2 + cntr, p3 + cntr, p4 + cntr, uv1, uv2, uv3, uv4);
	draw->BindTex2D(0);


	draw->SetDrawColor2D(0, 0, 1, 0.5);
	draw->DrawRect2D(pos.x, pos.y + size.y / 2 - CROSS_WIDTH, pos.x + size.x, pos.y + size.y / 2 + CROSS_WIDTH);
	draw->SetDrawColor2D(1, 0, 0, 0.5);
	draw->DrawRect2D(pos.x + size.x / 2 - CROSS_WIDTH, pos.y, pos.x + size.x / 2 + CROSS_WIDTH, pos.y + size.y);

	draw->SetDrawColor2D(1, 1, 1);
	glLineWidth(3);
	for (int i = 0; i < targets.size(); i++) {
		drawTarget(&targets[i], draw, &VP, zoom);
	}

	if (target) {
		glm::vec2 dir = glm::normalize(stop - start);
		if (target->primary) {
			dir = -dir;
		}
		start *= zoom;
		stop *= zoom;
		glLineWidth(2);
		draw->DrawLine2D(start.x + pos.x + size.x / 2, -start.y + pos.y + size.y / 2, stop.x + pos.x + size.x / 2, -stop.y + pos.y + size.y / 2);

		float dash_size = 5;
		stop = start - dir * dash_size;

		for (int i = 0; i < 35; i++) {
			stop += -dir * dash_size;
			start += -dir * dash_size;
			//if (start.x > size.x / 2 || start.y > size.y / 2 || start.x < -size.x / 2 || start.y < -size.y / 2) break;
			draw->DrawLine2D(start.x + pos.x + size.x / 2, -start.y + pos.y + size.y / 2, stop.x + pos.x + size.x / 2, -stop.y + pos.y + size.y / 2);
			stop += -dir * dash_size;
			start += -dir * dash_size;
		}
	}
	draw->BindTextShader();
	draw->SetTextColor(1, 1, 1);
	if (target) {
		draw->DrawString(f, target->name, pos.x, pos.y + 15);
	}
	//draw->DrawString(f, "zoom: " + std::to_string(zoom), pos.x, pos.y + 45);
	//std::string fixed = longLatFixed ? "(fixed)" : "";
	//draw->DrawString(f, "longLat: " + std::to_string(longLat.x) + " : " + std::to_string(longLat.y) + fixed, pos.x, pos.y + 90);


}
