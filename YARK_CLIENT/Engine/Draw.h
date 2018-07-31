#pragma once
#define GLEW_STATIC
#include <glew.h>
#include "Font.h"
#include <glm.hpp>
#include "Texture.h"
#include "Draw2DShader.h"
#include "TextShader.h"
#include "Draw3DShader.h"

void BindTexure(int slot, GLuint id);

class Draw {
private:
	int width, height;
	int currentOrthroMatID = 0, currentViewMatID = 0, currentProjMatID = 0;
	glm::mat4 orthroMat;
	glm::mat4 view;
	glm::mat4 proj; //should ideally be with orthromat due to screen size change 

	GLuint Dyn2DDrawVBOID = 0, Dyn2DDrawVAOID = 0;
	GLuint Dyn3DDrawVBOID = 0, Dyn3DDrawVAOID = 0;

	void *boundShader;

public:
	glm::mat4 GetOrthroMat();

	Draw2DShader * draw2DShader;
	TextShader *textShader;
	Draw3DShader *draw3DShader;

	Draw(int w, int h);
	void UpdateSize(int w, int h);
	void BindDraw3DShader(Draw3DShader *ds = NULL);
	void SetView3D(glm::mat4 view);
	void SetProj3D(glm::mat4 proj);
	void SetDrawColor3D(float r, float g, float b, float a = 1.f);
	void DrawTri3DUV(glm::vec3 p1, glm::vec2 uv1, glm::vec3 p2, glm::vec2 uv2, glm::vec3 p3, glm::vec2 uv3);
	void DrawTri3D(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
	void DrawTri3DLine(glm::vec3 p1, glm::vec3 p2);

	void BindDraw2DShader(Draw2DShader* ds = NULL);

	void BindTextShader(TextShader* ts = NULL);

	void BindTex2D(GLuint tex = NULL);
	void SetDrawColor2D(float r, float g, float b, float a = 1.f);
	void SetTextColor(float r, float g, float b, float a = 1.f);
	void DrawRect2D(float x1, float y1, float x2, float y2);
	void DrawRectUV2D(float x1, float y1, float x2, float y2, float u1, float v1, float u2, float v2);
	void DrawString(Font* f, std::string text, GLfloat x, GLfloat y, GLfloat scale = 1.f);
	void DrawChar(Font *f, char c, GLfloat x, GLfloat y, GLfloat scale = 1.f);
};