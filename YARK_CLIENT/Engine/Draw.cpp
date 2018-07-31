#include <iostream>
#include "Draw.h"
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/string_cast.hpp>

void BindTexure(int slot, GLuint id) {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, id);
}
glm::mat4 Draw::GetOrthroMat() {
	return orthroMat;
}

Draw::Draw(int w, int h) {
	draw2DShader = new Draw2DShader(LoadRawShader("Shaders/2DSimple.vert", "Shaders/2DSimple.frag"));
	textShader = new TextShader(LoadRawShader("Shaders/Text.vert", "Shaders/Text.frag"));
	draw3DShader = new Draw3DShader(LoadRawShader("Shaders/3dSimple.vert", "Shaders/3dSimple.frag"));

	glGenBuffers(1, &Dyn2DDrawVBOID);
	glBindBuffer(GL_ARRAY_BUFFER, Dyn2DDrawVBOID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &Dyn2DDrawVAOID);
	glBindVertexArray(Dyn2DDrawVAOID);
	glBindBuffer(GL_ARRAY_BUFFER, Dyn2DDrawVBOID);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (const void*)(sizeof(float) * 2));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	glGenBuffers(1, &Dyn3DDrawVBOID);
	glBindBuffer(GL_ARRAY_BUFFER, Dyn3DDrawVBOID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &Dyn3DDrawVAOID);
	glBindVertexArray(Dyn3DDrawVAOID);
	glBindBuffer(GL_ARRAY_BUFFER, Dyn3DDrawVBOID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (const void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	width = w;
	height = h;
	orthroMat = glm::ortho(0.f, float(width), float(height), 0.f, -1.f, 1.f);
}
void Draw::UpdateSize(int w, int h) {
	if (w != width || h != height) {
		currentOrthroMatID++;
		width = w;
		height = h;
		orthroMat = glm::ortho(0.f, float(width), float(height), 0.f);
	}
}
void Draw::BindTex2D(GLuint tex) {
	((Draw2DShader*)boundShader)->BindTex(tex);
}
void Draw::SetDrawColor2D(float r, float g, float b, float a) {
	((Draw2DShader*)boundShader)->SetColor(r, g, b, a);
}
void Draw::SetTextColor(float r, float g, float b, float a) {
	((TextShader*)boundShader)->SetColor(r, g, b, a);
}
void Draw::SetDrawColor3D(float r, float g, float b, float a) {
	((Draw3DShader*)boundShader)->SetColor(r, g, b, a);
}


void Draw::BindDraw3DShader(Draw3DShader *ds) {
	if (ds == NULL) {
		ds = draw3DShader;
	}
	boundShader = ds;
	glUseProgram(ds->shaderID);
	if (ds->projMatID != currentProjMatID) {
		ds->projMatID = currentProjMatID;
		glUniformMatrix4fv(ds->unifProj, 1, false, glm::value_ptr(proj));
	}
	if (ds->viewMatID != currentViewMatID) {
		ds->viewMatID = currentViewMatID;
		glUniformMatrix4fv(ds->unifView, 1, false, glm::value_ptr(view));
	}
}

void Draw::SetView3D(glm::mat4 newView) {
	currentViewMatID++;
	view = newView;
}
void Draw::SetProj3D(glm::mat4 newProj) {
	currentProjMatID++;
	proj = newProj;
}

void Draw::BindDraw2DShader(Draw2DShader* ds) {
	if (ds == NULL) {
		ds = draw2DShader;
	}
	boundShader = ds;
	glUseProgram(ds->shaderID);
	if (ds->orthroMatID != currentOrthroMatID) {
		ds->orthroMatID = currentOrthroMatID;
		glUniformMatrix4fv(ds->unifOrthro, 1, false, glm::value_ptr(orthroMat));
	}
}
void Draw::BindTextShader(TextShader* ts) {
	if (ts == NULL) {
		ts = textShader;
	}
	boundShader = ts;
	glUseProgram(ts->shaderID);
	if (ts->orthroMatID != currentOrthroMatID) {
		ts->orthroMatID = currentOrthroMatID;
		glUniformMatrix4fv(ts->unifOrthro, 1, false, glm::value_ptr(orthroMat));
	}
}

void Draw::DrawTri3DUV(glm::vec3 p1, glm::vec2 uv1, glm::vec3 p2, glm::vec2 uv2, glm::vec3 p3, glm::vec2 uv3) {
	GLfloat vertices[3][5] = {
		{ p1.x,p1.y,p1.z,uv1.x,uv1.y },
		{ p2.x,p2.y,p2.z,uv2.x,uv2.y },
		{ p3.x,p3.y,p3.z,uv3.x,uv3.y }
	};

	glBindBuffer(GL_ARRAY_BUFFER, Dyn3DDrawVBOID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(Dyn3DDrawVAOID);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
}
void Draw::DrawTri3DLine(glm::vec3 p1, glm::vec3 p2) {
	GLfloat vertices[2][5] = {
		{ p1.x,p1.y,p1.z,0,0 },
		{ p2.x,p2.y,p2.z,0,0 },
	};

	glBindBuffer(GL_ARRAY_BUFFER, Dyn3DDrawVBOID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(Dyn3DDrawVAOID);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}

void Draw::DrawTri3D(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
	GLfloat vertices[3][5] = {
		{ p1.x,p1.y,p1.z,0,0},
		{ p2.x,p2.y,p2.z,0,0},
		{ p3.x,p3.y,p3.z,0,0}
	};

	glBindBuffer(GL_ARRAY_BUFFER, Dyn3DDrawVBOID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(Dyn3DDrawVAOID);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);

}

void Draw::DrawString(Font *f, std::string text, GLfloat x, GLfloat y, GLfloat scale) {

	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = f->font[*c];

		GLfloat xpos = x + ch.bearingX * scale;
		GLfloat ypos = y - ch.bearingY* scale;

		GLfloat w = ch.sizeX * scale;
		GLfloat h = ch.sizeY * scale;

		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);

		DrawRect2D(xpos, ypos, xpos + w, ypos + h);

		x += (ch.Advance >> 6) * scale;
	}
}

void Draw::DrawChar(Font *f, char c, GLfloat x, GLfloat y, GLfloat scale) {

	Character ch = f->font[c];

	GLfloat xpos = x + ch.bearingX * scale;
	GLfloat ypos = y - ch.bearingY* scale;

	GLfloat w = ch.sizeX * scale;
	GLfloat h = ch.sizeY * scale;

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, ch.TextureID);

	DrawRect2D(xpos, ypos, xpos + w, ypos + h);
}

void Draw::DrawRect2D(float x1, float y1, float x2, float y2) {
	GLfloat vertices[6][4] = {
		{ x1,y2,0.0,1.0 },
		{ x1,y1,0.0,0.0 },
		{ x2,y1,1.0,0.0 },
		{ x1,y2,0.0,1.0 },
		{ x2,y1,1.0,0.0 },
		{ x2,y2,1.0,1.0 }
	};

	glBindBuffer(GL_ARRAY_BUFFER, Dyn2DDrawVBOID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(Dyn2DDrawVAOID);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Draw::DrawRectUV2D(float x1, float y1, float x2, float y2, float u1, float v1, float u2, float v2) {
	GLfloat vertices[6][4] = {
		{ x1,y2,u1,v2 },
		{ x1,y1,u1,v1 },
		{ x2,y1,u2,v1 },
		{ x1,y2,u1,v2 },
		{ x2,y1,u2,v1 },
		{ x2,y2,u2,v2 }
	};

	glBindBuffer(GL_ARRAY_BUFFER, Dyn2DDrawVBOID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(Dyn2DDrawVAOID);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}