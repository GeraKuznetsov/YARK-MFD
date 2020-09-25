#pragma once
#include "Shader.h"
#include "Font.h"

#define SHADER_2D 1
#define SHADER_3D 2
#define SHADER_TEXT 3

class Draw {
private:
	GLuint Shader2D, Shader3D, ShaderText;
	struct {
		GLuint unifOrthro, unifUseTex, unifColor, unifView;
		bool texEnabled;
		int oldOrthoID = -1;
	} Shader2DData;

	struct {
		GLuint unifProj, unifView, unifModel, unifColor, unifUseTex;
		bool texEnabled;
	} Shader3DData;

	struct {
		GLuint unifColor, unifOrthro, unifView;
		int oldOrthoID = -1;
	} ShaderTextData;

	int width = -1, height = -1;
	int OrthroMatID = -1;
	GLuint Dyn2DDrawVBOID = 0, Dyn2DDrawVAOID = 0;
	GLuint Dyn3DDrawVBOID = 0, Dyn3DDrawVAOID = 0;
	GLuint DynTextDrawVBOID = 0, DynTextDrawVAOID = 0;

public:
	glm::mat4 orthroMat;

	Draw(int w, int h);

	void SwitchShader(int type);
	glm::mat4 GetOrthroMat();
	void UpdateSize(int width, int height);

	//2D
	void BindTex2D(GLuint tex = NULL);
	void SetDrawColor2D(float r, float g, float b, float a = 1.f);
	void SetView2D(glm::mat4 mat = glm::mat4(1));
	void DrawLine2D(float x1, float y1, float x2, float y2);
	void DrawRect2D(float x1, float y1, float x2, float y2);
	void DrawRectUV2D(float x1, float y1, float x2, float y2, float u1, float v1, float u2, float v2);
	void DrawRectUV2D(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec2 p4, glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3, glm::vec2 uv4);

	//3D
	void BindTex3D(GLuint tex = NULL);
	void SetDrawColor3D(float r, float g, float b, float a = 1.f);
	void SetView3D(glm::mat4 view);
	void SetModel3D(glm::mat4 model);
	void SetProj3D(glm::mat4 proj);

	void DrawTri3DUV(glm::vec3 p1, glm::vec2 uv1, glm::vec3 p2, glm::vec2 uv2, glm::vec3 p3, glm::vec2 uv3);
	void DrawTri3D(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
	void DrawLine3D(glm::vec3 p1, glm::vec3 p2);
	void DrawPoint3D(glm::vec3 p1);
	void DrawTri3DLine(glm::vec3 p1, glm::vec3 p2);

	//TEXT
	void SetTextColor(float r, float g, float b, float a = 1.f);
	void SetViewText(glm::mat4 mat = glm::mat4(1));
	void DrawString(Font* f, std::string text, GLfloat x, GLfloat y, GLfloat scale = 1.f);
	void DrawChar(Font* f, char c, int x, int y, GLfloat scale = 1);
};