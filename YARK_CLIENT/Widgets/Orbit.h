#pragma once
#include "Widget.h"
#include "../Client/Client.h"
#include "Util/BodyInfo.h"

#define T_INITIAL 0
#define T_FINAL 1
#define T_ENCOUNTER 2
#define T_ESCAPE 3
#define T_MANEUVER 4
//? #define IMPACT 5

#define ORBIT_MAX_PATCHES 20

class OrbitDisplay : public Widget {
	bool LockView;
	struct View{
		glm::vec2 viewAngle;
		float zoom;
	};
	View views[NUM_BODIES];
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
		void PropogateOrbit(OrbitData* o, glm::vec3 col, bool forceSolid);
		~Orbit();
		void DrawPath(GLuint colorUnif);
		void DrawFlair(bool drawAN);
	};
	OrbitData* lockTo = 0;
	GLuint shader, viewUnif, projUnif, colorUnif;
	Orbit *equator, *primeMerid, *primeMerid2, *orthToCam;

	glm::vec3 V_AN, V_DN, T_AN, T_DN, V_CA, T_CA;

	//orbits
	Orbit *targetOrbit;
	int OrbitPatchesCount;
	Orbit *OrbitPatches[ORBIT_MAX_PATCHES];

	int SOI = 4, lastSOI = -1;

	//orbit list
	int orbitSelected = 0;
	int oly, olx, olc = 0;
	void OrbitListOption(XY pos, XY size, OrbitData *o, Orbit *orb, ManData *manD,int mID, OrbitData *next, ClosestAprouchData *CAD, char *forceName, OrbitData* prev);
	glm::vec3 GetCol(bool man, int i);
public:
	std::string GetTitle();
	~OrbitDisplay();
	OrbitDisplay();
	void Draw(XY pos, XY size);
};
