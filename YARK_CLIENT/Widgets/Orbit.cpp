#include "Orbit.h"
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#define PI ((double)(3.14159265358979323846264338327950288))

#undef min
#undef max

#if _WIN32
#define sprintf sprintf_s
#endif

OrbitDisplay::Orbit::Orbit(int vertCount) {
	this->vertCount = vertCount;
	glGenBuffers(1, &VBOID);
	glBindBuffer(GL_ARRAY_BUFFER, VBOID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * vertCount, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &VAOID);
	glBindVertexArray(VAOID);
	glBindBuffer(GL_ARRAY_BUFFER, VBOID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (const void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
}

OrbitDisplay::Orbit::~Orbit() {
	glDeleteBuffers(1, &VBOID);
	glDeleteBuffers(1, &VAOID);
}

void OrbitDisplay::Orbit::DrawPath(GLuint colorUnif) {
	if (selected) {
		glLineWidth(6);
	}
	else {
		glLineWidth(3);
	}
	glUniform4f(colorUnif, col.x, col.y, col.z, 1);
	glBindVertexArray(VBOID);
	glDrawArrays(renderType, 0, vertCount);
	glBindVertexArray(0);
}

void OrbitDisplay::Orbit::DrawFlair(Draw* draw, bool drawAN) {
	draw->SetDrawColor3D(1, 0, 1);
	if (isFinal && drawAN) {
		draw->DrawPoint3D(AN);
		draw->DrawPoint3D(DN);
		if (selected) {
			glLineWidth(4);
		}
		else {
			glLineWidth(2);
		}
		draw->DrawTri3DLine(AN, glm::normalize(AN) * pRad);
		draw->DrawTri3DLine(DN, glm::normalize(DN) * pRad);
	}

	draw->SetDrawColor3D(0, 1, 0);
	if (renderPE) draw->DrawPoint3D(PE);
	if (renderAP) draw->DrawPoint3D(AP);


	draw->SetDrawColor3D(1, 0, 0);
	draw->DrawPoint3D(Craft);
	if (!isFinal) {
		draw->DrawPoint3D(EN);
	}
}

OrbitDisplay::OrbitDisplay(WidgetStuff ws) : Widget(ws) {
	shader = LoadRawShader("Shaders/line.vert", "Shaders/line.frag");
	glUseProgram(shader);
	viewUnif = glGetUniformLocation(shader, "view");
	projUnif = glGetUniformLocation(shader, "proj");
	colorUnif = glGetUniformLocation(shader, "color");

	View v = View{ glm::vec2(0, 0), 1.f };

	planets.push_back(Planet{ "Kerbol",     261600000.f,     115000000000.f,(float) 1.757e28, v });
	planets.push_back(Planet{ "Moho",       250000.f  ,      9646.66f ,    (float) 2.526e21, v });
	planets.push_back(Planet{ "Eve",        700000.f  ,      85109.36f,    (float) 1.224e23, v });
	planets.push_back(Planet{ "Gilly",      13000.f  ,       126.12f,      (float) 1.242e17, v });
	planets.push_back(Planet{ "Kerbin",     600000.f,        84159.29f,    (float) 5.292e22, v });
	planets.push_back(Planet{ "Mun",        200000.f  ,      2429.56f,     (float) 9.760e20, v });
	planets.push_back(Planet{ "Minmus",     60000.f,         2247.43f,     (float) 2.646e19, v });
	planets.push_back(Planet{ "Duna",       320000.f ,       47921.95f,    (float) 4.515e21, v });
	planets.push_back(Planet{ "Ike",        130000.f  ,      1049.60f,     (float) 2.782e20, v });
	planets.push_back(Planet{ "Dres",       138000.f ,       32832.84f,    (float) 3.219e20, v });
	planets.push_back(Planet{ "Jool",       6000000.f,       2455985.19f,  (float) 4.233e24, v });
	planets.push_back(Planet{ "Laythe",     500000.f,        3723.65f,     (float) 2.940e22, v });
	planets.push_back(Planet{ "Vall",       300000.f ,       2406.40f,     (float) 3.109e21, v });
	planets.push_back(Planet{ "Tylo",       600000.f ,       10856.52f,    (float) 4.233e22, v });
	planets.push_back(Planet{ "Bop",        65000.f   ,      1221.06f,     (float) 3.726e19, v });
	planets.push_back(Planet{ "Pol",        44000.f   ,      1042.14f ,    (float) 1.081e19, v });
	planets.push_back(Planet{ "Eeloo",      210000.f,        119082.94f,   (float) 1.115e21, v });

	//planetCircle = MakeCircle(0, 0, 1);
	equator = new Orbit(360);
	primeMerid = new Orbit(360);
	primeMerid2 = new Orbit(360);
	targetOrbit = new Orbit(360);

	for (int i = 0; i < ORBIT_MAX_PATCHES; i++) {
		OrbitPatches[i] = new Orbit(360);
	}
}

glm::dvec3 kep_2_cart(double semi_latus_rectum, double anomaly, double eccentricity, float inclination, float arg_PE, double long_AN) {
	glm::dvec3 posPQW_;
	glm::dvec3 posECI_;
	posPQW_ = glm::vec3(semi_latus_rectum * glm::cos(anomaly) / (1 + eccentricity * glm::cos(anomaly)), semi_latus_rectum * glm::sin(anomaly) / (1 + eccentricity * glm::cos(anomaly)), 0);

	double cO = glm::cos(arg_PE); double sO = glm::sin(arg_PE);
	//double cO = glm::cos(long_AN); double sO = glm::sin(long_AN);
	double co = glm::cos(long_AN); double so = glm::sin(long_AN);
	//double co = glm::cos(arg_PE); double so = glm::sin(arg_PE);
	double ci = glm::cos(inclination);     double si = glm::sin(inclination);
	glm::dmat3 dcmPQW2ECI_ = glm::dmat3(cO * co - sO * ci*so, -cO * so - sO * ci*co, sO*si,
		sO*co + cO * ci*so, -sO * so + cO * ci*co, -cO * si,
		si*so, si*co, ci);

	posECI_ = dcmPQW2ECI_ * posPQW_;
	return glm::vec3(-posECI_.x, posECI_.y, posECI_.z);
}

void OrbitDisplay::Orbit::PropogateOrbit(OrbitData* o, std::vector<Planet> planets, glm::vec3 col, bool forceSolid) {
	pRad = planets[o->SOINumber].rad;
	isFinal = o->transEnd == T_FINAL;
	this->col = col;
	float data[360 * 4];
	double dANom = (o->anomolyEnd - o->anomoly) / 360;
	double anom = o->anomoly;
	for (int i = 0; i < 360; i++) {
		glm::vec3 pos = kep_2_cart(o->SemiLatusRectum, -anom, o->e, glm::radians(o->inc), glm::radians(o->argOfPE), glm::radians(o->longOfAscNode));
		anom += dANom;
		data[i * 4] = pos.x;
		data[i * 4 + 1] = pos.y;
		data[i * 4 + 2] = pos.z;

		double height = glm::length(pos);
		float strength = 1;

		if (!forceSolid) {
			if (o->transStart == T_INITIAL && o->transEnd == T_FINAL) {
				strength = float(i) / 360.f / 2.f + 0.5f;
			}

			if (height < planets[o->SOINumber].rad) {
				strength = 0;
			}
		}
		data[i * 4 + 3] = strength;
	}

	Craft = kep_2_cart(o->SemiLatusRectum, -o->anomoly, o->e, glm::radians(o->inc), glm::radians(o->argOfPE), glm::radians(o->longOfAscNode));
	PE = kep_2_cart(o->SemiLatusRectum, 0, o->e, glm::radians(o->inc), glm::radians(o->argOfPE), glm::radians(o->longOfAscNode));
	if (o->transEnd == T_FINAL) {
		renderPE = true;
	}
	else {
		if (o->anomoly > 0) {
			renderPE = o->anomolyEnd > 2 * PI;
		}
		else {
			renderPE = (o->anomolyEnd > 0);
		}
	}
	renderPE = renderPE && (glm::length(PE) > planets[o->SOINumber].rad);
	renderAP = (o->anomolyEnd > PI);
	AP = kep_2_cart(o->SemiLatusRectum, PI, o->e, glm::radians(o->inc), glm::radians(o->argOfPE), glm::radians(o->longOfAscNode));
	EN = kep_2_cart(o->SemiLatusRectum, -o->anomolyEnd, o->e, glm::radians(o->inc), glm::radians(o->argOfPE), glm::radians(o->longOfAscNode));
	AN = kep_2_cart(o->SemiLatusRectum, glm::radians(o->argOfPE), o->e, glm::radians(o->inc), glm::radians(o->argOfPE), glm::radians(o->longOfAscNode));
	DN = kep_2_cart(o->SemiLatusRectum, glm::radians(o->argOfPE) - PI, o->e, glm::radians(o->inc), glm::radians(o->argOfPE), glm::radians(o->longOfAscNode));
	if (forceSolid) {
		renderType = GL_LINE_LOOP;
	}
	else {
		renderType = (o->transEnd == T_FINAL) ? GL_LINE_LOOP : GL_LINE_STRIP;
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBOID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 4 * vertCount, data);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OrbitDisplay::Tick(Draw* draw) {
	WindowUpdate(draw);

	draw->BindDraw2DShader();
	draw->SetDrawColor2D(0, 0, 0);
	draw->DrawRect2D(pos.x, pos.y, pos.x + size.x, pos.y + size.y);

	float rad = (glm::min(size.x, size.y) - 100) / 2;
	Planet p = planets[SOI];

	if (mouseInWindow && win->MouseDown(3)) {
		planets[SOI].view.viewAngle += glm::vec2(win->MouseDXY()) * glm::vec2(0.5);
	}
	if (win->KeyDown(SDL_SCANCODE_W)) {
		planets[SOI].view.viewAngle.y++;
	}
	if (win->KeyDown(SDL_SCANCODE_S)) {
		planets[SOI].view.viewAngle.y--;
	}
	if (win->KeyDown(SDL_SCANCODE_A)) {
		planets[SOI].view.viewAngle.x++;
	}
	if (win->KeyDown(SDL_SCANCODE_D)) {
		planets[SOI].view.viewAngle.x--;
	}
	if (win->KeyDown(SDL_SCANCODE_E)) {
		planets[SOI].view.zoom += planets[SOI].view.zoom / 8;
	}
	if (win->KeyDown(SDL_SCANCODE_Q)) {
		planets[SOI].view.zoom -= planets[SOI].view.zoom / 8;
	}
	if (planets[SOI].view.viewAngle.y > 90) {
		planets[SOI].view.viewAngle.y = 90;
	}
	else if (planets[SOI].view.viewAngle.y < -90) {
		planets[SOI].view.viewAngle.y = -90;
	}
	if (planets[SOI].view.viewAngle.x < 0) {
		planets[SOI].view.viewAngle.x += 360;
	}
	if (planets[SOI].view.viewAngle.x > 360) {
		planets[SOI].view.viewAngle.x -= 360;
	}

	bool hasTarget = client->Vessel.HasTarget;
	planets[SOI].view.zoom += win->getMouseWheelDelta().y * planets[SOI].view.zoom / 8;
	float scale = 1.f / p.soiDist * planets[SOI].view.zoom;
	if (p.rad * scale > rad) {
		scale = 1.f / p.soiDist * (planets[SOI].view.zoom = rad / p.rad * p.soiDist);
	}

	OrbitData* lastPatch = 0;
	if (client->OrbitPlan.PlannedOrbitPatches.size()) {
		lastPatch = &client->OrbitPlan.PlannedOrbitPatches[client->OrbitPlan.PlannedOrbitPatches.size() - 1];
	}
	else {
		if (client->OrbitPlan.CurrentOrbitPatches.size()) {
			lastPatch = &client->OrbitPlan.CurrentOrbitPatches[client->OrbitPlan.CurrentOrbitPatches.size() - 1];
		}
	}

	OrbitData* o = &client->Vessel.CurrentOrbit;
	OrbitRecalcFrameSkip++;
	if ((OrbitRecalcFrameSkip % (60 / 15)) == 0 || SOI != lastSOI) {
		if (SOI != lastSOI) {
			OrbitData o_;
			o_.SOINumber = SOI;
			o_.e = 0;
			o_.SemiLatusRectum = p.rad;
			o_.inc = 0;
			o_.argOfPE = o_.longOfAscNode = 0;
			o_.anomoly = 0; o_.anomolyEnd = PI * 2;
			glm::vec3 red = glm::vec3(1, 0, 0);
			equator->PropogateOrbit(&o_, planets, red, true);

			o_.inc = 90;
			primeMerid->PropogateOrbit(&o_, planets, red, true);

			o_.longOfAscNode = 90;
			primeMerid2->PropogateOrbit(&o_, planets, red, true);
		}

		lastSOI = SOI;
		OrbitPatchesCount = 0;
		for (int i = 0; i < client->OrbitPlan.CurrentOrbitPatches.size(); i++) {
			if (client->OrbitPlan.CurrentOrbitPatches[i].SOINumber == SOI) {
				OrbitPatches[OrbitPatchesCount]->PropogateOrbit(&client->OrbitPlan.CurrentOrbitPatches[i], planets, GetCol(false, i), false);
				OrbitPatchesCount++;
			}
		}
		for (int i = 0; i < client->OrbitPlan.PlannedOrbitPatches.size(); i++) {
			if (client->OrbitPlan.PlannedOrbitPatches[i].SOINumber == SOI) {
				OrbitPatches[OrbitPatchesCount]->PropogateOrbit(&client->OrbitPlan.PlannedOrbitPatches[i], planets, GetCol(true, i), false);
				OrbitPatchesCount++;
			}
		}
		if (hasTarget && client->OrbitPlan.TargetOrbit.SOINumber == SOI) {
			targetOrbit->PropogateOrbit(&client->OrbitPlan.TargetOrbit, planets, glm::vec3(1, 0, 1), false);

			OrbitData *targetOrbit = &client->OrbitPlan.TargetOrbit;
			V_AN = kep_2_cart(lastPatch->SemiLatusRectum, -client->OrbitPlan.CAD.ANAnom, lastPatch->e, glm::radians(lastPatch->inc), glm::radians(lastPatch->argOfPE), glm::radians(lastPatch->longOfAscNode));
			T_AN = kep_2_cart(targetOrbit->SemiLatusRectum, -client->OrbitPlan.CAD.TargetANAnom, targetOrbit->e, glm::radians(targetOrbit->inc), glm::radians(targetOrbit->argOfPE), glm::radians(targetOrbit->longOfAscNode));

			V_DN = kep_2_cart(lastPatch->SemiLatusRectum, -client->OrbitPlan.CAD.ANAnom - PI, lastPatch->e, glm::radians(lastPatch->inc), glm::radians(lastPatch->argOfPE), glm::radians(lastPatch->longOfAscNode));
			T_DN = kep_2_cart(targetOrbit->SemiLatusRectum, -client->OrbitPlan.CAD.TargetANAnom - PI, targetOrbit->e, glm::radians(targetOrbit->inc), glm::radians(targetOrbit->argOfPE), glm::radians(targetOrbit->longOfAscNode));

			V_CA = kep_2_cart(lastPatch->SemiLatusRectum, -client->OrbitPlan.CAD.CAAnom, lastPatch->e, glm::radians(lastPatch->inc), glm::radians(lastPatch->argOfPE), glm::radians(lastPatch->longOfAscNode));
			T_CA = kep_2_cart(targetOrbit->SemiLatusRectum, -client->OrbitPlan.CAD.TargetCAAnom, targetOrbit->e, glm::radians(targetOrbit->inc), glm::radians(targetOrbit->argOfPE), glm::radians(targetOrbit->longOfAscNode));
		}
	}

	glm::vec2 centr = glm::vec2(pos) + glm::vec2(size.x / 2, size.y / 2);
	glm::mat4 orthroMat = glm::ortho(float(-centr.x), float(win->getSize().x - centr.x), float(win->getSize().y - centr.y), float(-centr.y), -1000000.f, 1000000.f);

	//glm::mat4 view = glm::translate(glm::mat4(1), glm::vec3(-0, 0, -002));
	glm::mat4 view = glm::rotate(glm::mat4(1), glm::radians(planets[SOI].view.viewAngle.y + 90), glm::vec3(1, 0, 0));
	view = glm::rotate(view, glm::radians(planets[SOI].view.viewAngle.x), glm::vec3(0, 0, 1));
	view = glm::scale(view, glm::vec3(scale));
	//float radius = 50;

	/*draw->SetDrawColor3D(1, 0.5, 0.5, 0.5);
	draw->DrawTri3D(glm::vec3(p.rad, 0, p.rad), glm::vec3(-p.rad, 0, p.rad), glm::vec3(-p.rad, 0, -p.rad));
	draw->DrawTri3D(glm::vec3(-p.rad, 0, -p.rad), glm::vec3(p.rad, 0, -p.rad), glm::vec3(p.rad, 0, p.rad));

	glm::vec3 offset = glm::vec3(0, 0, 0);

	draw->SetDrawColor3D(1, 0, 0);
	draw->DrawTri3DLine(glm::vec3(0, 0, 0) + offset, glm::vec3(radius, 0, 0) + offset);
	draw->SetDrawColor3D(0, 1, 0);
	draw->DrawTri3DLine(glm::vec3(0, 0, 0) + offset, glm::vec3(0, radius, 0) + offset);
	draw->SetDrawColor3D(0, 0, 1);
	draw->DrawTri3DLine(glm::vec3(0, 0, 0) + offset, glm::vec3(0, 0, radius) + offset);*/
	glUseProgram(shader);
	SetShaderMat4(projUnif, orthroMat);
	SetShaderMat4(viewUnif, view);

	primeMerid->DrawPath(colorUnif);
	primeMerid2->DrawPath(colorUnif);
	equator->DrawPath(colorUnif);

	for (int i = 0; i < OrbitPatchesCount; i++) {
		OrbitPatches[i]->DrawPath(colorUnif);
	}
	if (hasTarget && client->OrbitPlan.TargetOrbit.SOINumber == SOI) {
		targetOrbit->DrawPath(colorUnif);
	}

	draw->SetView3D(view);
	draw->SetProj3D(orthroMat);
	draw->BindDraw3DShader();
	draw->draw3DShader->BindTex();

	glPointSize(6);
	for (int i = 0; i < OrbitPatchesCount; i++) {
		OrbitPatches[i]->DrawFlair(draw, !hasTarget);
	}

	if (hasTarget && client->OrbitPlan.TargetOrbit.SOINumber == SOI) {
		targetOrbit->DrawFlair(draw, false);
	}
	if (win->KeyRepeating(SDL_SCANCODE_UP) && orbitSelected != 0) {
		orbitSelected--;
	}
	if (win->KeyRepeating(SDL_SCANCODE_DOWN)) {
		orbitSelected++;
	}
	if (orbitSelected > olc - 1) {
		orbitSelected = olc - 1;
	}
	if (orbitSelected < 0) {
		orbitSelected = 0;
	}

	if (hasTarget && lastPatch->SOINumber == SOI && lastPatch->transEnd == T_FINAL) {
		glLineWidth(2);
		draw->SetDrawColor3D(1, 0, 1);
		draw->DrawTri3DLine(T_AN, V_DN);
		draw->DrawTri3DLine(T_DN, V_AN);
		draw->SetDrawColor3D(0, 0.5, 1);
		draw->DrawPoint3D(V_CA);
		draw->DrawPoint3D(T_CA);
	}

	olc = 0;
	olx = pos.x + size.x - 120;
	oly = pos.y + 15;

	for (int i = 0; i < OrbitPatchesCount; i++) {
		OrbitPatches[i]->selected = false;
	}
	targetOrbit->selected = false;

	draw->BindTextShader();
	draw->SetTextColor(1, 1, 1);
	draw->DrawString(f, "==Current Orbit==", olx - f->GetTextWidth("==Current Orbit==") / 2, oly); oly += 15;

	int OCount = 0;
	for (int i = 0; i < client->OrbitPlan.CurrentOrbitPatches.size(); i++) {
		OrbitData *next = (i == (client->OrbitPlan.CurrentOrbitPatches.size() - 1)) ? 0 : &client->OrbitPlan.CurrentOrbitPatches[i + 1];
		ClosestAprouchData *CAD = ((i == client->OrbitPlan.CurrentOrbitPatches.size() - 1) && (client->OrbitPlan.PlannedOrbitPatches.size() == 0) && hasTarget) ? &client->OrbitPlan.CAD : 0;
		OrbitListOption(draw, &client->OrbitPlan.CurrentOrbitPatches[i], OrbitPatches[OCount], 0, next, CAD, 0);
		if (client->OrbitPlan.CurrentOrbitPatches[i].SOINumber == SOI) {
			OCount++;
		}
	}

	int MCount = 0;
	oly += 15;
	if (hasTarget) {
		draw->SetTextColor(1, 1, 1);
		draw->DrawString(f, "==Target==", olx - f->GetTextWidth("==Target==") / 2, oly); oly += 15;
		OrbitListOption(draw, &client->OrbitPlan.TargetOrbit, targetOrbit, 0, 0, 0, (char *)client->OrbitPlan.TargetName.c_str()); oly += 15;
	}

	if (client->OrbitPlan.PlannedOrbitPatches.size() != 0) {
		draw->SetTextColor(1, 1, 1);
		draw->DrawString(f, "==Planned Orbit==", olx - f->GetTextWidth("==Planned Orbit==") / 2, oly); oly += 15;
		for (int i = 0; i < client->OrbitPlan.PlannedOrbitPatches.size(); i++) {
			OrbitData *next = (i == (client->OrbitPlan.PlannedOrbitPatches.size() - 1)) ? 0 : &client->OrbitPlan.PlannedOrbitPatches[i + 1];
			ManData *md = 0;
			ClosestAprouchData *CAD = ((i == client->OrbitPlan.PlannedOrbitPatches.size() - 1) && hasTarget) ? &client->OrbitPlan.CAD : 0;
			if (client->OrbitPlan.PlannedOrbitPatches[i].transStart == T_MANEUVER) {
				md = &client->OrbitPlan.Mans[MCount];
				MCount++;
			}
			OrbitListOption(draw, &client->OrbitPlan.PlannedOrbitPatches[i], OrbitPatches[OCount], md, next, CAD, 0);
			if (client->OrbitPlan.PlannedOrbitPatches[i].SOINumber == SOI) {
				OCount++;
			}
		}
	}
}

glm::vec3 OrbitDisplay::GetCol(bool man, int i) {
	if (man) {
		return glm::vec3(1, 1 - float(i) / float(client->OrbitPlan.PlannedOrbitPatches.size()), 0);
	}
	else {
		return glm::vec3(0, float(i) / float(client->OrbitPlan.PlannedOrbitPatches.size()), 1);
	}

}

void OrbitDisplay::OrbitListOption(Draw *draw, OrbitData* o, Orbit *orb, ManData *manD, OrbitData* next, ClosestAprouchData *CAD, char *forceName) {
	std::string text;
	if (forceName) {
		text = forceName;
	}
	else {
		std::string nextName = next ? planets[next->SOINumber].name : "?";
		switch (o->transEnd) {
		case T_FINAL:
			text = "Final (" + planets[o->SOINumber].name + ")";
			break;
		case T_ENCOUNTER:
			text = nextName + " Encounter";
			break;
		case T_ESCAPE:
			text = "Escape to " + nextName;
			break;
		case T_MANEUVER:
			text = "Transfer";
			break;
		default:
			printf("%d\n", o->transStart);
			text = "foo";
		}
		if (o->transStart == T_MANEUVER) {
			text = "(M)" + text;
		}
	}

	char buff[64];

	if (orbitSelected == olc) {
		orb->selected = true;
		draw->SetTextColor(1, 1, 1);
		int sy = pos.y + 15;
		draw->DrawString(f, "SOI: " + planets[SOI = o->SOINumber].name, pos.x + 10, sy); sy += 15;
		if (olc == 0) {
			sprintf(buff, "Alt: %.0f m", client->Vessel.Alt);
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
		}
		draw->DrawString(f, "Anomoly : " + std::to_string(o->anomoly), pos.x + 10, sy); sy += 15;
		if (o->transEnd != T_FINAL) {
			draw->DrawString(f, "Patch EndAnomoly : " + std::to_string(o->anomolyEnd), pos.x + 10, sy); sy += 15;

			sprintf(buff, "Time To Patch End: %d s", o->T2PatchEnd);
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
		}
		sprintf(buff, "SLR: %.0f m", o->SemiLatusRectum);
		draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
		sprintf(buff, "Inc: %.2f deg", o->inc);
		draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
		sprintf(buff, "LAN: %.2f deg", o->longOfAscNode);
		draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
		sprintf(buff, "AOP: %.2f deg", o->argOfPE);
		draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
		draw->DrawString(f, "Ecentricity: " + std::to_string(o->e), pos.x + 10, sy); sy += 15;
		if (o->transEnd == T_FINAL) {
			sprintf(buff, "Period: %d s", o->period);
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
		}
		if (orb->renderAP) {
			sprintf(buff, "AP: %.0f m", o->AP);
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
			int t2ap = 0;
			if (o->anomoly > 0) {
				t2ap = o->T2Pe - o->period / 2;
			}
			else {
				t2ap = o->T2Pe + o->period / 2;
			}
			sprintf(buff, "T2 AP: %d s", t2ap);
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
		}
		if (orb->renderPE) {
			sprintf(buff, "PE: %.0f m", o->PE);
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
			sprintf(buff, "T2 PE: %d s", o->T2Pe);
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
		}
		if (orb->isFinal) {
			if (o->T2DN < o->T2AN) {
				sprintf(buff, "T2 DN: %d s", o->T2DN);
			}
			else {
				sprintf(buff, "T2 AN: %d s", o->T2AN);
			}
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
		}
		if (CAD) {
			sy += 15;
			sprintf(buff, "Rel Inc: %.2f deg", CAD->RelInc);
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
			if (CAD->T2DN < CAD->T2AN) {
				sprintf(buff, "T2 rel. DN: %d s", CAD->T2DN);
			}
			else {
				sprintf(buff, "T2 rel. AN: %d s", CAD->T2AN);
			}
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;

			sprintf(buff, "T2 CA: %d s", CAD->T2CA);
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
			sprintf(buff, "Sep @ CA: %d m", CAD->SepAtCA);
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;


			//sprintf(buff, "Rel Inc: %f deg", CAD.);
			//draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;

		}
		if (o->transStart == T_MANEUVER) {
			sy += 15; sy += 15;
			sprintf(buff, "Man dV: %.1f m/s", manD->DV);
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
			sprintf(buff, "Time To Man: %d s", int(manD->UT - client->Vessel.UT));
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
			sprintf(buff, "dV-Grade: %.2f m/s", manD->Z);
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
			sprintf(buff, "dV-Normal: %.2f m/s", manD->Y);
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
			sprintf(buff, "dV-Radial: %.2f m/s", manD->X);
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
			printf("%f\n", float(client->Vessel.UT));
		}
		draw->SetTextColor(1, 1, 0);
		text = ">" + text;
	}
	else {
		draw->SetTextColor(1, 1, 1);
	}
	draw->DrawString(f, text, olx - f->GetTextWidth(text) / 2, oly);
	oly += 15;
	olc++;
}


OrbitDisplay::~OrbitDisplay() {
	delete equator;
	delete primeMerid;
	delete 	primeMerid2;
	for (int i = 0; i < ORBIT_MAX_PATCHES; i++) {
		delete OrbitPatches[i];
	}
}
