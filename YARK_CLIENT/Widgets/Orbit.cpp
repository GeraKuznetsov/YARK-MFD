#include "Orbit.h"
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include "Util/OrbitCalc.h"

#undef min
#undef max

#if not _WIN32
#define sprintf_s sprintf
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

void OrbitDisplay::Orbit::DrawFlair(bool drawAN) {
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

OrbitDisplay::OrbitDisplay() {
	shader = LoadSHader("Shaders/line.vert", "Shaders/line.frag");
	glUseProgram(shader);
	viewUnif = glGetUniformLocation(shader, "view");
	projUnif = glGetUniformLocation(shader, "proj");
	colorUnif = glGetUniformLocation(shader, "color");

	View v = View{ glm::vec2(0, 0), 1.f };
	for (int i = 0; i < NUM_BODIES; i++)views[i] = v;

	//planetCircle = MakeCircle(0, 0, 1);
	equator = new Orbit(360);
	primeMerid = new Orbit(360);
	primeMerid2 = new Orbit(360);
	targetOrbit = new Orbit(360);
	orthToCam = new Orbit(360);

	for (int i = 0; i < ORBIT_MAX_PATCHES; i++) {
		OrbitPatches[i] = new Orbit(360);
	}
}

void OrbitDisplay::Orbit::PropogateOrbit(OrbitData* o, glm::vec3 col, bool forceSolid) {
	pRad = Bodies[o->SOINumber].rad;
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

			if (height < Bodies[o->SOINumber].rad) {
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
	renderPE = renderPE && (glm::length(PE) > Bodies[o->SOINumber].rad);
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

void OrbitDisplay::Draw(XY pos, XY size) {
	draw->SwitchShader(SHADER_2D);
	draw->SetDrawColor2D(0, 0, 0);
	draw->DrawRect2D(pos.x, pos.y, pos.x + size.x, pos.y + size.y);


	float rad = (glm::min(size.x, size.y) - 100) / 2;
	BodyInfo p = Bodies[SOI];
	bool mouseInWindow = win->MouseX() > pos.x && win->MouseX() < pos.x + size.x && win->MouseY() > pos.y && win->MouseY() < pos.y + size.y;
	if (mouseInWindow && win->MouseDown(3)) {
		views[SOI].viewAngle += glm::vec2(win->MouseDXY()) * glm::vec2(0.5);
	}
	if (win->KeyDown(SDL_SCANCODE_W)) {
		views[SOI].viewAngle.y++;
	}
	if (win->KeyDown(SDL_SCANCODE_S)) {
		views[SOI].viewAngle.y--;
	}
	if (win->KeyDown(SDL_SCANCODE_A)) {
		views[SOI].viewAngle.x++;
	}
	if (win->KeyDown(SDL_SCANCODE_D)) {
		views[SOI].viewAngle.x--;
	}
	if (win->KeyDown(SDL_SCANCODE_E)) {
		views[SOI].zoom += views[SOI].zoom / 8;
	}
	if (win->KeyDown(SDL_SCANCODE_Q)) {
		views[SOI].zoom -= views[SOI].zoom / 8;
	}
	if (views[SOI].viewAngle.y > 90) {
		views[SOI].viewAngle.y = 90;
	}
	else if (views[SOI].viewAngle.y < -90) {
		views[SOI].viewAngle.y = -90;
	}
	if (views[SOI].viewAngle.x < 0) {
		views[SOI].viewAngle.x += 360;
	}
	if (views[SOI].viewAngle.x > 360) {
		views[SOI].viewAngle.x -= 360;
	}
	if (LockView && lockTo) {
		views[SOI].viewAngle.y = 90 - abs(lockTo->inc);
		views[SOI].viewAngle.x = lockTo->longOfAscNode + 180;
	}
	bool hasTarget = client.Vessel.HasTarget;
	views[SOI].zoom += win->getMouseWheelDelta().y * views[SOI].zoom / 8;
	float scale = 1.f / p.soiDist * views[SOI].zoom;
	if (p.rad * scale > rad) {
		scale = 1.f / p.soiDist * (views[SOI].zoom = rad / p.rad * p.soiDist);
	}

	OrbitData* lastPatch = 0;
	if (client.orbitPlan.PlannedOrbitPatches.size()) {
		lastPatch = &client.orbitPlan.PlannedOrbitPatches[client.orbitPlan.PlannedOrbitPatches.size() - 1];
	}
	else { //TODO: WHAT? causing NPE when have target but no path to it
		if (client.orbitPlan.CurrentOrbitPatches.size()) {
			lastPatch = &client.orbitPlan.CurrentOrbitPatches[client.orbitPlan.CurrentOrbitPatches.size() - 1];
		}
	}

	OrbitData* o = &client.Vessel.CurrentOrbit;
		OrbitData o_;
		o_.SOINumber = SOI;
		o_.e = 0;
		o_.SemiLatusRectum = p.rad;
		o_.inc = 0;
		o_.argOfPE = o_.longOfAscNode = 0;
		o_.anomoly = 0; o_.anomolyEnd = PI * 2;
		glm::vec3 red = glm::vec3(1, 0, 0);
		equator->PropogateOrbit(&o_, red, true);

		o_.inc = 90;
		primeMerid->PropogateOrbit(&o_, red, true);

		o_.longOfAscNode = 90;
		primeMerid2->PropogateOrbit(&o_, red, true);

		o_.inc = 90 - views[SOI].viewAngle.y;
		o_.longOfAscNode = -views[SOI].viewAngle.x + 180;
		orthToCam->PropogateOrbit(&o_, red, true);

	lastSOI = SOI;
	OrbitPatchesCount = 0;
	for (int i = 0; i < client.orbitPlan.CurrentOrbitPatches.size(); i++) {
		if (client.orbitPlan.CurrentOrbitPatches[i].SOINumber == SOI) {
			OrbitPatches[OrbitPatchesCount]->PropogateOrbit(&client.orbitPlan.CurrentOrbitPatches[i], GetCol(false, i), false);
			OrbitPatchesCount++;
		}
	}
	for (int i = 0; i < client.orbitPlan.PlannedOrbitPatches.size(); i++) {
		if (client.orbitPlan.PlannedOrbitPatches[i].SOINumber == SOI) {
			OrbitPatches[OrbitPatchesCount]->PropogateOrbit(&client.orbitPlan.PlannedOrbitPatches[i], GetCol(true, i), false);
			OrbitPatchesCount++;
		}
	}
	if (hasTarget && client.orbitPlan.TargetOrbit.SOINumber == SOI) {
		targetOrbit->PropogateOrbit(&client.orbitPlan.TargetOrbit, glm::vec3(1, 0, 1), false);

		OrbitData* targetOrbit = &client.orbitPlan.TargetOrbit;
		V_AN = kep_2_cart(lastPatch->SemiLatusRectum, -client.orbitPlan.CAD.ANAnom, lastPatch->e, glm::radians(lastPatch->inc), glm::radians(lastPatch->argOfPE), glm::radians(lastPatch->longOfAscNode));
		T_AN = kep_2_cart(targetOrbit->SemiLatusRectum, -client.orbitPlan.CAD.TargetANAnom, targetOrbit->e, glm::radians(targetOrbit->inc), glm::radians(targetOrbit->argOfPE), glm::radians(targetOrbit->longOfAscNode));

		V_DN = kep_2_cart(lastPatch->SemiLatusRectum, -client.orbitPlan.CAD.ANAnom - PI, lastPatch->e, glm::radians(lastPatch->inc), glm::radians(lastPatch->argOfPE), glm::radians(lastPatch->longOfAscNode));
		T_DN = kep_2_cart(targetOrbit->SemiLatusRectum, -client.orbitPlan.CAD.TargetANAnom - PI, targetOrbit->e, glm::radians(targetOrbit->inc), glm::radians(targetOrbit->argOfPE), glm::radians(targetOrbit->longOfAscNode));

		V_CA = kep_2_cart(lastPatch->SemiLatusRectum, -client.orbitPlan.CAD.CAAnom, lastPatch->e, glm::radians(lastPatch->inc), glm::radians(lastPatch->argOfPE), glm::radians(lastPatch->longOfAscNode));
		T_CA = kep_2_cart(targetOrbit->SemiLatusRectum, -client.orbitPlan.CAD.TargetCAAnom, targetOrbit->e, glm::radians(targetOrbit->inc), glm::radians(targetOrbit->argOfPE), glm::radians(targetOrbit->longOfAscNode));
	}

	glm::vec2 centr = glm::vec2(pos) + glm::vec2(size.x / 2, size.y / 2);
	glm::mat4 orthroMat = glm::ortho(float(-centr.x), float(win->getSize().x - centr.x), float(win->getSize().y - centr.y), float(-centr.y), -1000000.f, 1000000.f);

	//glm::mat4 view = glm::translate(glm::mat4(1), glm::vec3(-0, 0, -002));
	glm::mat4 view = glm::rotate(glm::mat4(1), glm::radians(views[SOI].viewAngle.y + 90), glm::vec3(1, 0, 0));
	view = glm::rotate(view, glm::radians(views[SOI].viewAngle.x), glm::vec3(0, 0, 1));
	view = glm::scale(view, glm::vec3(scale));

	glUseProgram(shader);
	SetShaderMat4(projUnif, orthroMat);
	SetShaderMat4(viewUnif, view);

	primeMerid->DrawPath(colorUnif);
	primeMerid2->DrawPath(colorUnif);
	equator->DrawPath(colorUnif);
	orthToCam->DrawPath(colorUnif);

	for (int i = 0; i < OrbitPatchesCount; i++) {
		OrbitPatches[i]->DrawPath(colorUnif);
	}
	if (hasTarget && client.orbitPlan.TargetOrbit.SOINumber == SOI) {
		targetOrbit->DrawPath(colorUnif);
	}

	draw->SwitchShader(SHADER_3D);
	draw->SetView3D(view);
	draw->SetProj3D(orthroMat);
	draw->BindTex3D();

	glPointSize(6);
	for (int i = 0; i < OrbitPatchesCount; i++) {
		OrbitPatches[i]->DrawFlair(!hasTarget);
	}

	if (hasTarget && client.orbitPlan.TargetOrbit.SOINumber == SOI) {
		targetOrbit->DrawFlair(false);
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

	//TODO fix
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

	draw->SwitchShader(SHADER_TEXT);
	draw->SetTextColor(1, 1, 1);
	std::string lockText = LockView ? "[V] - Unlock View" : "[V] - Lock View To Normal";
	draw->DrawString(f, lockText, pos.x + (size.x - f->GetTextWidth(lockText)) / 2, pos.y + 20);

	if (win->KeyTyped(SDL_SCANCODE_V)) {
		LockView ^= 1;
	}
	draw->DrawString(f, "==Current Orbit==", olx - f->GetTextWidth("==Current Orbit==") / 2, oly); oly += 15;

	int OCount = 0;
	OrbitData* prev = 0;
	for (int i = 0; i < client.orbitPlan.CurrentOrbitPatches.size(); i++) {
		OrbitData* next = (i == (client.orbitPlan.CurrentOrbitPatches.size() - 1)) ? 0 : &client.orbitPlan.CurrentOrbitPatches[i + 1];
		ClosestAprouchData* CAD = ((i == client.orbitPlan.CurrentOrbitPatches.size() - 1) && (client.orbitPlan.PlannedOrbitPatches.size() == 0) && hasTarget) ? &client.orbitPlan.CAD : 0;
		OrbitListOption(pos, size, &client.orbitPlan.CurrentOrbitPatches[i], OrbitPatches[OCount], 0, 0, next, CAD, 0, prev);
		prev = &client.orbitPlan.CurrentOrbitPatches[i];
		if (client.orbitPlan.CurrentOrbitPatches[i].SOINumber == SOI) {
			OCount++;
		}
	}

	int MCount = 0;
	oly += 15;
	if (hasTarget) {
		draw->SetTextColor(1, 1, 1);
		draw->DrawString(f, "==Target==", olx - f->GetTextWidth("==Target==") / 2, oly); oly += 15;
		OrbitListOption(pos, size, &client.orbitPlan.TargetOrbit, targetOrbit, 0, 0, 0, 0, (char*)client.orbitPlan.TargetName.c_str(), 0); oly += 15;
	}

	if (client.orbitPlan.PlannedOrbitPatches.size() != 0) {
		draw->SetTextColor(1, 1, 1);
		draw->DrawString(f, "==Planned Orbit==", olx - f->GetTextWidth("==Planned Orbit==") / 2, oly); oly += 15;
		OrbitData* prev = 0;

		for (int i = 0; i < client.orbitPlan.PlannedOrbitPatches.size(); i++) {

			OrbitData* next = (i == (client.orbitPlan.PlannedOrbitPatches.size() - 1)) ? 0 : &client.orbitPlan.PlannedOrbitPatches[i + 1];
			ManData* md = 0;
			ClosestAprouchData* CAD = ((i == client.orbitPlan.PlannedOrbitPatches.size() - 1) && hasTarget) ? &client.orbitPlan.CAD : 0;
			if (client.orbitPlan.PlannedOrbitPatches[i].transStart == T_MANEUVER) {
				md = &client.orbitPlan.Mans[MCount];
				MCount++;
			}
			OrbitListOption(pos, size, &client.orbitPlan.PlannedOrbitPatches[i], OrbitPatches[OCount], md, MCount - 1, next, CAD, 0, prev);
			prev = &client.orbitPlan.PlannedOrbitPatches[i];
			if (client.orbitPlan.PlannedOrbitPatches[i].SOINumber == SOI) {
				OCount++;
			}
		}
	}
}

glm::vec3 OrbitDisplay::GetCol(bool man, int i) {
	if (man) {
		return glm::vec3(1, 1 - float(i) / float(client.orbitPlan.PlannedOrbitPatches.size()), 0);
	}
	else {
		return glm::vec3(0, float(i) / float(client.orbitPlan.PlannedOrbitPatches.size()), 1);
	}

}

void OrbitDisplay::OrbitListOption(XY pos, XY size, OrbitData* o, Orbit* orb, ManData* manD, int mID, OrbitData* next, ClosestAprouchData* CAD, char* forceName, OrbitData* prev) {
	std::string text;
	if (forceName) {
		text = forceName;
	}
	else {
		std::string nextName = next ? Bodies[next->SOINumber].name : "?";
		switch (o->transEnd) {
		case T_FINAL:
			text = "Final (" + Bodies[o->SOINumber].name + ")";
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
			text = "foo";
		}
		if (o->transStart == T_MANEUVER) {
			text = "(M)" + text;
		}
	}

	char buff[64];

	if (orbitSelected == olc) {
		lockTo = o;
		orb->selected = true;
		draw->SetTextColor(1, 1, 1);
		int sy = pos.y + 15;
		draw->DrawString(f, "SOI: " + Bodies[SOI = o->SOINumber].name, pos.x + 10, sy); sy += 15;
		if (olc == 0) {
			sprintf_s(buff, "Alt: %.0f m", client.Vessel.Alt);
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
		}
		draw->DrawString(f, "Anomoly : " + std::to_string(o->anomoly), pos.x + 10, sy); sy += 15;
		if (o->transEnd != T_FINAL) {
			draw->DrawString(f, "Patch EndAnomoly : " + std::to_string(o->anomolyEnd), pos.x + 10, sy); sy += 15;

			sprintf_s(buff, "Time To Patch End: %d s", o->T2PatchEnd);
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
		}
		sprintf_s(buff, "SLR: %.0f m", o->SemiLatusRectum);
		draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
		sprintf_s(buff, "Inc: %.2f deg", o->inc);
		draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
		sprintf_s(buff, "LAN: %.2f deg", o->longOfAscNode);
		draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
		sprintf_s(buff, "AOP: %.2f deg", o->argOfPE);
		draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
		draw->DrawString(f, "Ecentricity: " + std::to_string(o->e), pos.x + 10, sy); sy += 15;
		if (o->transEnd == T_FINAL) {
			sprintf_s(buff, "Period: %d s", o->period);
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
		}
		if (orb->renderAP) {
			sprintf_s(buff, "AP: %.0f m", o->AP);
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
			int t2ap = 0;
			if (o->anomoly > 0) {
				t2ap = o->T2Pe - o->period / 2;
			}
			else {
				t2ap = o->T2Pe + o->period / 2;
			}
			sprintf_s(buff, "T2 AP: %d s", t2ap);
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
		}
		if (orb->renderPE) {
			sprintf_s(buff, "PE: %.0f m", o->PE);
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
			sprintf_s(buff, "T2 PE: %d s", o->T2Pe);
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
		}
		if (orb->isFinal) {
			if (o->T2DN < o->T2AN) {
				sprintf_s(buff, "T2 DN: %d s", o->T2DN);
			}
			else {
				sprintf_s(buff, "T2 AN: %d s", o->T2AN);
			}
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
		}
		if (CAD) {
			sy += 15;
			sprintf_s(buff, "Rel Inc: %.2f deg", CAD->RelInc);
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
			if (CAD->T2DN < CAD->T2AN) {
				sprintf_s(buff, "T2 rel. DN: %d s", CAD->T2DN);
			}
			else {
				sprintf_s(buff, "T2 rel. AN: %d s", CAD->T2AN);
			}
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;

			sprintf_s(buff, "T2 CA: %d s", CAD->T2CA);
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
			sprintf_s(buff, "Sep @ CA: %d m", CAD->SepAtCA);
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;


			//sprintf_s(buff, "Rel Inc: %f deg", CAD.);
			//draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;

		}
		draw->DrawString(f, "[C] - Create Manuever", pos.x + 20, pos.y + size.y - 40); {}
		if (win->KeyTyped(SDL_SCANCODE_C)) {
			if (prev) {
				client.SendManChange(1, mID, (o->T2PatchEnd + prev->T2PatchEnd) / 2 + client.Vessel.UT, glm::vec3());
			}
			else {
				client.SendManChange(1, mID, client.Vessel.UT + o->period / 2, glm::vec3());
			}
		}
		if (o->transStart == T_MANEUVER) {
			sy += 15; sy += 15;
			sprintf_s(buff, "Man dV: %.1f m/s", manD->DV);
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
			sprintf_s(buff, "Time To Man: %d s", int(manD->UT - client.Vessel.UT));
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
			sprintf_s(buff, "dV-Grade: %.2f m/s", manD->Z);
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
			sprintf_s(buff, "dV-Normal: %.2f m/s", manD->Y);
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
			sprintf_s(buff, "dV-Radial: %.2f m/s", manD->X);
			draw->DrawString(f, buff, pos.x + 10, sy); sy += 15;
			draw->DrawString(f, "Manuever Controls: [X] - Delete [H/N] - Prograde [J/L] - radial [I/K] - Normal [U/O] - Time", pos.x + 20, pos.y + size.y - 20);
			bool mod = false;
			if (win->KeyTyped(SDL_SCANCODE_X)) {
				client.SendManChange(2, mID, 0, glm::vec3());
			}
			else {
				float speed = win->KeyDown(SDL_SCANCODE_LSHIFT) ? 10 : 1;
				glm::vec3 change = glm::vec3(manD->X, manD->Y, manD->Z);
				double UT = manD->UT;
				if (win->KeyDown(SDL_SCANCODE_L)) {
					mod = true;
					change.x += speed;
				}
				else if (win->KeyDown(SDL_SCANCODE_J)) {
					mod = true;
					change.x -= speed;
				}
				if (win->KeyDown(SDL_SCANCODE_H)) {
					mod = true;
					change.z += speed;
				}
				else if (win->KeyDown(SDL_SCANCODE_N)) {
					mod = true;
					change.z -= speed;
				}
				if (win->KeyDown(SDL_SCANCODE_I)) {
					mod = true;
					change.y += speed;
				}
				else if (win->KeyDown(SDL_SCANCODE_K)) {
					mod = true;
					change.y -= speed;
				}
				if (win->KeyDown(SDL_SCANCODE_O)) {
					mod = true;
					UT += speed;
				}
				else if (win->KeyDown(SDL_SCANCODE_U)) {
					mod = true;
					UT -= speed;
				}
				if (mod) {
					client.SendManChange(0, mID, UT, change);
				}
			}
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

std::string OrbitDisplay::GetTitle() {
	return "Orbit Display";
}

OrbitDisplay::~OrbitDisplay() {
	delete equator;
	delete primeMerid;
	delete 	primeMerid2;
	delete 	orthToCam;
	for (int i = 0; i < ORBIT_MAX_PATCHES; i++) {
		delete OrbitPatches[i];
	}
}
