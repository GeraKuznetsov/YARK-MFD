#include "GroundTrack.h"
#include "Util/BodyInfo.h"
std::string GroundTrack::GetTitle() {
	return "Ground Track";
}

GroundTrack::GroundTrack() {
	glGenBuffers(1, &gtVBO);
	glGenVertexArrays(1, &gtVAO);
	glBindVertexArray(gtVAO);
	glBindBuffer(GL_ARRAY_BUFFER, gtVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (const void*)(sizeof(float) * 2));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
}

struct PlanetRot {
	float startRot, rotRate;
};

PlanetRot PlanetRots[NUM_BODIES] = {
	PlanetRot{0,1}, //kerbal
	PlanetRot{190,1210000.0}, //moho
	PlanetRot{0, 80500.0}, //eve
	PlanetRot{5, 28255.0}, //gilly
	PlanetRot{90,21549.425}, //kerbin
	PlanetRot{230,138984.38}, //mun
	PlanetRot{230,40400.0}, //minmus
	PlanetRot{90,65517.859}, //duna
	PlanetRot{0,65517.862}, //ike
	PlanetRot{25,34800.0}, //dres
	PlanetRot{0,36000.0}, //jool
	PlanetRot{90,52980.879}, //laythe
	PlanetRot{0, 105962.09 }, //vall
	PlanetRot{0, 211926.36}, //tylo
	PlanetRot{230,544507.43}, //bop
	PlanetRot{25, 901902.62 }, //pol
	PlanetRot{25,19460.0}, //eedloo
};

float planetRot(int planet, float UT) {
	float per = (UT - trunc(UT / PlanetRots[planet].rotRate) * PlanetRots[planet].rotRate) / PlanetRots[planet].rotRate;
	per = per * 360 + PlanetRots[planet].startRot;
	if (per > 360.f) per -= 360.f;
	return per;
}

glm::vec2 CalcPoint(OrbitData* orbit, float trueAnom, float UT, glm::vec3* coord3D = 0) {
	glm::vec3 XYZ = kep_2_cart(orbit->SemiLatusRectum, -trueAnom, orbit->e, glm::radians(orbit->inc), glm::radians(orbit->argOfPE), glm::radians(orbit->longOfAscNode));
	if (coord3D) *coord3D = XYZ;
	XYZ = glm::normalize(XYZ);
	float X = (-atan2(XYZ.x, XYZ.y) - PI / 2 - glm::radians(planetRot(orbit->SOINumber, UT)));
	float Y = -asin(XYZ.z);
	return glm::vec2(X, Y);
}

glm::vec2 CalcPoint(XY pos, XY size, OrbitData* orbit, float trueAnom, float UT, glm::vec3* coord3D = 0) {
	glm::vec2 v = CalcPoint(orbit, trueAnom, UT, coord3D);
	v.x /= (PI * 2);
	v.y /= PI;
	while (v.x > 0.5) v.x -= 1;
	while (v.x < -0.5) v.x += 1;
	v.x *= size.x;
	v.y *= size.y;
	v.x += pos.x + size.x / 2;
	v.y += pos.y + size.y / 2;
	return v;
}

struct vertex {
	float x, y, a, b;
};
std::vector <vertex> lineVerts;

void GroundTrack::DrawOrbit(XY pos, XY size, OrbitData* orbit, float startTime, float startMA, glm::vec3 color, int periods, bool dots) {
	int scale = 360 * 3;
	int drawNum;

	lineVerts.clear();
	float delta = PI * 2.f / scale;

	for (int i = 0; i < scale * periods; i++) {
		drawNum = i;
		float meanAnomaly = delta * double(i) + startMA;
		float trueAnomaly = GetTrueAnomaly(orbit, solveEccentricAnomaly(meanAnomaly, orbit->e, 1E-07, 8));
		float UTHere = startTime + (meanAnomaly / 2 / PI * orbit->period);
		glm::vec3 xyz;
		glm::vec2 c = CalcPoint(pos, size, orbit, trueAnomaly, UTHere, &xyz);
		if (glm::length(xyz) < Bodies[orbit->SOINumber].rad) break;
			lineVerts.push_back(vertex{ c.x, c.y, 0, 0 });
	}


	draw->SwitchShader(SHADER_2D);
	draw->BindTex2D();
	draw->SetDrawColor2D(color.x, color.y, color.z);
	if (VBOsize < lineVerts.size()) {
		VBOsize = lineVerts.size();
		glBindBuffer(GL_ARRAY_BUFFER, gtVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * lineVerts.size(), lineVerts.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	else {
		glBindBuffer(GL_ARRAY_BUFFER, gtVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex) * lineVerts.size(), lineVerts.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(gtVAO);
	glDrawArrays(GL_LINE_STRIP, 0, lineVerts.size());
	glBindVertexArray(0);

	if (!dots) return;
	draw->SetDrawColor2D(0, 1, 0);
	draw->BindTex2D();
	for (int i = 0; i < periods; i++) {
		glm::vec2 PE = CalcPoint(pos, size, orbit, 0, client.Vessel.UT + orbit->T2Pe + i * orbit->period);
		draw->SwitchShader(SHADER_2D);
		draw->SetDrawColor2D(1, 1, 0);
		draw->DrawRect2D(PE.x - 2, PE.y - 2, PE.x + 2, PE.y + 2);
		draw->SwitchShader(SHADER_TEXT);
		draw->SetTextColor(1, 1, 0);
		std::string text = "P" + std::to_string(i);
		draw->DrawString(f, text, PE.x - f->GetTextWidth(text) / 2, PE.y + 20);
	}

	for (int i = 0; i < periods; i++) {
		glm::vec2 A;
		if (orbit->T2Pe > orbit->period / 2) {
			A = CalcPoint(pos, size, orbit, PI, client.Vessel.UT + orbit->T2Pe - orbit->period / 2 + i * orbit->period);
		}
		else {
			A = CalcPoint(pos, size, orbit, PI, client.Vessel.UT + orbit->T2Pe + orbit->period / 2 + i * orbit->period);
		}
		draw->SwitchShader(SHADER_2D);
		draw->SetDrawColor2D(0, 1, 1);
		draw->DrawRect2D(A.x - 2, A.y - 2, A.x + 2, A.y + 2);
		draw->SwitchShader(SHADER_TEXT);
		draw->SetTextColor(0, 1, 1);
		std::string text = "A" + std::to_string(i);
		draw->DrawString(f, text, A.x - f->GetTextWidth(text) / 2, A.y + 20);
	}

	for (int i = 0; i < periods + 1; i++) {
		glm::vec2 T = CalcPoint(pos, size, orbit, orbit->anomoly, client.Vessel.UT + i * orbit->period);
		draw->SwitchShader(SHADER_2D);
		draw->SetDrawColor2D(0, 1, 0);
		draw->DrawRect2D(T.x - 2, T.y - 2, T.x + 2, T.y + 2);
		draw->SwitchShader(SHADER_TEXT);
		draw->SetTextColor(0, 1, 0);
		std::string text = "T" + std::to_string(i);
		draw->DrawString(f, text, T.x - f->GetTextWidth(text) / 2, T.y + 20);
	}
}

void GroundTrack::Draw(XY pos, XY size) {
	glLineWidth(2);
	VesselPacket VP = client.Vessel;
	int SOI = VP.CurrentOrbit.SOINumber;
	GLuint mapTexture = TL.getPlanetTextureLow(SOI);
	draw->SwitchShader(SHADER_2D);
	draw->BindTex2D(mapTexture);
	draw->SetDrawColor2D(1, 1, 1);
	draw->DrawRect2D(pos.x, pos.y, pos.x + size.x, pos.y + size.y);

	if (client.orbitPlan.CurrentOrbitPatches.size()) {
		OrbitData* orbit = &client.orbitPlan.CurrentOrbitPatches[client.orbitPlan.CurrentOrbitPatches.size() - 1];

		float StartMeanAnom = GetMeanAnomaly(orbit, GetEccentricAnomaly(orbit, orbit->anomoly));
		if (StartMeanAnom < 0) StartMeanAnom += PI * 2;
		float timeSpentOnOrbit = StartMeanAnom / 2 / PI * orbit->period;

		DrawOrbit(pos, size, orbit, client.Vessel.UT - timeSpentOnOrbit, StartMeanAnom, glm::vec3(1, 0, 0), 5, 1);

		if (client.orbitPlan.PlannedOrbitPatches.size()) {
			OrbitData* manOrbit = &client.orbitPlan.PlannedOrbitPatches[0];
			StartMeanAnom = GetMeanAnomaly(manOrbit, GetEccentricAnomaly(manOrbit, manOrbit->anomoly));
			timeSpentOnOrbit = StartMeanAnom / 2 / PI * manOrbit->period;
			DrawOrbit(pos, size, manOrbit, client.orbitPlan.Mans[0].UT - timeSpentOnOrbit, StartMeanAnom, glm::vec3(1, 1, 0), 5, false);
		}
	}
}