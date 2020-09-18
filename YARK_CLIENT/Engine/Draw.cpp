#include "Draw.h"
#include <gtc\matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <string>

#pragma pack(push, 1)
struct TextVert {
	float vert[2];
	float uv[2];
	uint32_t id;
};
#pragma pop

glm::mat4 Draw::GetOrthroMat() {
	return orthroMat;
}

Draw::Draw(int w, int h) {
	glUseProgram(Shader2D = LoadSHader("Shaders/2DSimple.vert", "Shaders/2DSimple.frag"));
	Shader2DData.unifOrthro = glGetUniformLocation(Shader2D, "orthro");
	Shader2DData.unifUseTex = glGetUniformLocation(Shader2D, "useTex");
	Shader2DData.unifColor = glGetUniformLocation(Shader2D, "color");
	Shader2DData.unifView = glGetUniformLocation(Shader2D, "view");
	Shader2DData.texEnabled = false;
	glUniform1i(Shader2DData.unifUseTex, 0);
	glUniform1i(glGetUniformLocation(Shader2D, "tex"), 0);
	SetView2D(glm::mat4(1));
	SetDrawColor2D(1, 1, 1);

	glUseProgram(Shader3D = LoadSHader("Shaders/3DSimple.vert", "Shaders/3DSimple.frag"));
	Shader3DData.unifProj = glGetUniformLocation(Shader3D, "proj");
	Shader3DData.unifView = glGetUniformLocation(Shader3D, "view");
	Shader3DData.unifModel = glGetUniformLocation(Shader3D, "model");
	Shader3DData.unifColor = glGetUniformLocation(Shader3D, "color");
	Shader3DData.unifUseTex = glGetUniformLocation(Shader3D, "useTex");
	Shader3DData.texEnabled = false;
	glUniform1i(Shader3DData.unifUseTex, 0);
	glUniform1i(glGetUniformLocation(Shader3D, "tex"), 0);
	SetModel3D(glm::mat4(1));

	glUseProgram(ShaderText = LoadSHader("Shaders/Text.vert", "Shaders/Text.frag"));
	ShaderTextData.unifColor = glGetUniformLocation(ShaderText, "color");
	ShaderTextData.unifOrthro = glGetUniformLocation(ShaderText, "orthro");
	ShaderTextData.unifView = glGetUniformLocation(ShaderText, "view");
	SetViewText();
	SetTextColor(1, 1, 1);
	glUniform1i(glGetUniformLocation(ShaderText, "tex"), 0);

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

	glGenBuffers(1, &DynTextDrawVBOID);
	glBindBuffer(GL_ARRAY_BUFFER, DynTextDrawVBOID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TextVert) * 6, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &DynTextDrawVAOID);
	glBindVertexArray(DynTextDrawVAOID);
	glBindBuffer(GL_ARRAY_BUFFER, DynTextDrawVBOID);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TextVert), (const void*)offsetof(TextVert, vert));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TextVert), (const void*)offsetof(TextVert, uv));
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(TextVert), (const void*)offsetof(TextVert, id));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	UpdateSize(w, h);
}

void Draw::UpdateSize(int w, int h) {
	if (w != width || h != height) {
		OrthroMatID++;
		width = w;
		height = h;
		orthroMat = glm::ortho(0.f, float(width), float(height), 0.f);
	}
}

//2D

void Draw::SwitchShader(int type) {
	switch (type) {
	case SHADER_2D:
		glUseProgram(Shader2D);
		if (Shader2DData.oldOrthoID != OrthroMatID) {
			Shader2DData.oldOrthoID = OrthroMatID;
			glUniformMatrix4fv(Shader2DData.unifOrthro, 1, false, glm::value_ptr(orthroMat));
		}
		break;

	case SHADER_TEXT:
		glUseProgram(ShaderText);
		if (ShaderTextData.oldOrthoID != OrthroMatID) {
			ShaderTextData.oldOrthoID = OrthroMatID;
			glUniformMatrix4fv(ShaderTextData.unifOrthro, 1, false, glm::value_ptr(orthroMat));
		}
		break;
	case SHADER_3D:
		glUseProgram(Shader3D);
		break;
	}
}

void Draw::SetView2D(glm::mat4 mat) {
	glUniformMatrix4fv(Shader2DData.unifView, 1, false, glm::value_ptr(mat));
}

void Draw::BindTex2D(GLuint tex) {
	bool enabled = tex != 0;
	if (Shader2DData.texEnabled != enabled) {
		glUniform1i(Shader2DData.unifUseTex, enabled);
	}
	if (enabled) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
	}
	Shader2DData.texEnabled = enabled;
}

void Draw::BindTex3D(GLuint tex) {
	bool enabled = tex != 0;
	if (Shader3DData.texEnabled != enabled) {
		glUniform1i(Shader3DData.unifUseTex, enabled);
	}
	if (enabled) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
	}
	Shader3DData.texEnabled = enabled;
}

void Draw::SetDrawColor2D(float r, float g, float b, float a) {
	glUniform4f(Shader2DData.unifColor, r, g, b, a);
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

void Draw::DrawRectUV2D(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec2 p4, glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3, glm::vec2 uv4) {
	GLfloat vertices[6][4] = {
	{ p1.x,p1.y,uv1.x,uv1.y },
	{ p4.x,p4.y,uv4.x,uv4.y },
	{ p2.x,p2.y,uv2.x,uv2.y },
	{ p4.x,p4.y,uv4.x,uv4.y },
	{ p3.x,p3.y,uv3.x,uv3.y },
	{ p2.x,p2.y,uv2.x,uv2.y }
	};

	glBindBuffer(GL_ARRAY_BUFFER, Dyn2DDrawVBOID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(Dyn2DDrawVAOID);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

}
void Draw::DrawLine2D(float x1, float y1, float x2, float y2) {
	GLfloat vertices[6][4] = {
		{ x1,y1,0.0,0.0 },
		{ x2,y2,0.0,0.0 },
		{ 0,0,0.0,0.0 },
		{ 0,0,0.0,0.0 },
		{ 0,0,0.0,0.0 },
		{ 0,0,0.0,0.0 }
	};

	glBindBuffer(GL_ARRAY_BUFFER, Dyn2DDrawVBOID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(Dyn2DDrawVAOID);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);

}


//3D

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

void Draw::SetDrawColor3D(float r, float g, float b, float a) {
	glUniform4f(Shader3DData.unifColor, r, g, b, a);
}

void Draw::SetView3D(glm::mat4 view) {
	glUniformMatrix4fv(Shader3DData.unifView, 1, false, glm::value_ptr(view));
}

void Draw::SetModel3D(glm::mat4 model) {
	glUniformMatrix4fv(Shader3DData.unifModel, 1, false, glm::value_ptr(model));
}

void Draw::SetProj3D(glm::mat4 proj) {
	glUniformMatrix4fv(Shader3DData.unifProj, 1, false, glm::value_ptr(proj));
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
void Draw::DrawLine3D(glm::vec3 p1, glm::vec3 p2) {
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
		{ p1.x,p1.y,p1.z,0,0 },
	{ p2.x,p2.y,p2.z,0,0 },
	{ p3.x,p3.y,p3.z,0,0 }
	};

	glBindBuffer(GL_ARRAY_BUFFER, Dyn3DDrawVBOID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(Dyn3DDrawVAOID);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);

}

void Draw::DrawPoint3D(glm::vec3 p1) {
	GLfloat vertices[1][5] = {
		{ p1.x,p1.y,p1.z,0,0}
	};

	glBindBuffer(GL_ARRAY_BUFFER, Dyn3DDrawVBOID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(Dyn3DDrawVAOID);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);

}

//TEXT
void Draw::SetTextColor(float r, float g, float b, float a) {
	glUniform4f(ShaderTextData.unifColor, r, g, b, a);
}

void Draw::DrawString(Font* f, std::string text, GLfloat x, GLfloat y, GLfloat scale) {
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		char index = *c;

		Character ch = f->chars[index];

		GLfloat xpos = x + ch.bearingX * scale;
		GLfloat ypos = y - ch.bearingY * scale;

		GLfloat w = f->size * scale;
		GLfloat h = f->size * scale;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, f->atlas);

		TextVert vertices[6] = {
			{{xpos, ypos}, {0,0},(uint32_t)index},
			{{xpos, ypos + h}, {0,1},(uint32_t)index},
			{{xpos + w, ypos + h}, {1,1}, (uint32_t)index},
			{{xpos, ypos}, {0,0},(uint32_t)index},
			{{xpos + w, ypos + h}, {1,1},(uint32_t)index},
			{{xpos + h, ypos}, {1,0},(uint32_t)index},
		};

		glBindBuffer(GL_ARRAY_BUFFER, DynTextDrawVBOID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(DynTextDrawVAOID);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		x += (ch.Advance >> 6) * scale;
	}
}

void Draw::DrawChar(Font* f, char c, int x, int y, GLfloat scale) {
	Character ch = f->chars[c];

	GLfloat xpos = x + ch.bearingX * scale;
	GLfloat ypos = y - ch.bearingY * scale;

	GLfloat w = f->size * scale;
	GLfloat h = f->size * scale;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, f->atlas);

	TextVert vertices[6] = {
		{{xpos, ypos}, {0,0},(uint32_t)c},
		{{xpos, ypos + h}, {0,1},(uint32_t)c},
		{{xpos + w, ypos + h}, {1,1}, (uint32_t)c},
		{{xpos, ypos}, {0,0},(uint32_t)c},
		{{xpos + w, ypos + h}, {1,1},(uint32_t)c},
		{{xpos + h, ypos}, {1,0},(uint32_t)c},
	};

	glBindBuffer(GL_ARRAY_BUFFER, DynTextDrawVBOID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(DynTextDrawVAOID);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Draw::SetViewText(glm::mat4 mat) {
	glUniformMatrix4fv(ShaderTextData.unifView, 1, false, glm::value_ptr(mat));
}