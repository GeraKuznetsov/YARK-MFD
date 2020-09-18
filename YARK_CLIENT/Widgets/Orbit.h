#pragma once
#include "Widget.h"
#include "../Client/Client.h"

#define T_INITIAL 0
#define T_FINAL 1
#define T_ENCOUNTER 2
#define T_ESCAPE 3
#define T_MANEUVER 4
//? #define IMPACT 5

#define ORBIT_MAX_PATCHES 20

class OrbitDisplay : public Widget {
	struct View{
		glm::vec2 viewAngle;
		float zoom;
	};
	struct Planet {
		std::string name;
		float rad;
		float soiDist;
		float mass;
		View View;
	};
	class Orbit {
		float pRad;
	public:
		bool selected = false;
		int vertCount;
		GLuint VBOID = 0, VAOID = 0;
		bool isFinal;
		glm::vec3 col;
		GLuint renderType;
		bool renderAP, renderPE;
		glm::vec3 Craft, AP, PE, EN, AN, DN;
		Orbit(int vertCount);
		void PropogateOrbit(OrbitData* o, std::vector<Planet> planets, glm::vec3 col, bool forceSolid);
		~Orbit();
		void DrawPath(GLuint colorUnif);
		void DrawFlair(bool drawAN);
	};
	GLuint shader, viewUnif, projUnif, colorUnif;
	Orbit *equator, *primeMerid, *primeMerid2;

	glm::vec3 V_AN, V_DN, T_AN, T_DN, V_CA, T_CA;

	//orbits
	Orbit *targetOrbit;
	int OrbitPatchesCount;
	Orbit *OrbitPatches[ORBIT_MAX_PATCHES];

	int OrbitRecalcFrameSkip = 0;
	int SOI = 4, lastSOI = -1;

	//orbit list
	int orbitSelected = 0;
	int oly, olx, olc = 0;
	void OrbitListOption(XY pos, OrbitData *o, Orbit *orb, ManData *manD, OrbitData *next, ClosestAprouchData *CAD, char *forceName);
	glm::vec3 GetCol(bool man, int i);
public:
	std::string GetTitle();
	~OrbitDisplay();
	std::vector<Planet> planets;
	OrbitDisplay();
	void Draw(XY pos, XY size);
};