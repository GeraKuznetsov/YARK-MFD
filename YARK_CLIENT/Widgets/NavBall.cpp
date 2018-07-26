#include "NavBall.h"

#define BORDER_SCALE 1.05f

NavBall::NavBall(XY pos, XY size, std::string title, Font* font, Client** client, Cam* cam) : Widget(pos, size, title, font) {
	this->client = client;
	this->cam = cam;
	shader = LoadRawShader("Shaders/sphere.vert", "Shaders/sphere.frag");
	glUseProgram(shader);
	proj = glGetUniformLocation(shader, "proj");
	model = glGetUniformLocation(shader, "model");
	rot = glGetUniformLocation(shader, "rot");

	GLfloat vertices[6][2] = {
		{-1.f*BORDER_SCALE,-1.f*BORDER_SCALE },
		{ -1.f*BORDER_SCALE,1.f*BORDER_SCALE},
		{ 1.f*BORDER_SCALE,-1.f*BORDER_SCALE},
		{ 1.f*BORDER_SCALE,-1.f*BORDER_SCALE},
		{ -1.f*BORDER_SCALE,1.f*BORDER_SCALE},
		{ 1.f*BORDER_SCALE,1.f*BORDER_SCALE }
	};

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vao);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);


	navballTex = new Texture("Tex/navball/navball.png");
	chevron = new Texture("Tex/navball/chevron.png");
	size = XY{ 400,400 };
}

#include "gtc\matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/quaternion.hpp"

void NavBall::Tick(Draw* draw) {
	RenderWindow(draw);

	DataIn DI = (*client) ? (*client)->dataIn : DataIn();

	draw->BindTextShader();
	draw->SetDrawColor2D(0, 1, 0);

	draw->DrawString(f, std::to_string((DI.Prograde.Heading)), 10, 30);
	draw->DrawString(f, std::to_string((DI.Prograde.Pitch)), 10, 60);
	
	draw->DrawString(f, std::to_string(DI.Pitch), 10, 90);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, navballTex->textureID);

	float rad = (size.x-200) / 2;

	glUseProgram(shader);
	cam->Calculate();
	SetShaderMat4(proj, cam->projection);

	glm::mat4 modelMat = glm::mat4(1);
	modelMat = glm::translate(modelMat, glm::vec3(pos.x + size.x / 2, pos.y + size.y / 2, 0));
	modelMat = glm::scale(modelMat, glm::vec3(rad/ BORDER_SCALE, rad/ BORDER_SCALE, rad/ BORDER_SCALE));
	SetShaderMat4(model, modelMat);

	glm::mat4 rotMat = glm::mat4(1);

	rotMat = glm::rotate(rotMat, glm::radians(-DI.Heading + 90), glm::vec3(0, 0, 1));
	rotMat = glm::rotate(rotMat, glm::radians(DI.Pitch - 90), glm::vec3(1, 0, 0));
	rotMat = rotMat * glm::rotate(glm::mat4(1), glm::radians(-DI.Roll), glm::vec3(0, 0, 1));

	SetShaderMat4(rot, rotMat);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	draw->BindDraw2DShader();
	draw->SetDrawColor2D(1, 1, 1);
	draw->BindTex2D(chevron->textureID);

	draw->DrawRectUV2D(pos.x + size.x / 2 - 90, pos.y + size.y / 2 - 35, pos.x + size.x / 2 + 90, pos.y + size.y / 2 + 35, 0.f / 256.f, 0.f / 256.f, 180.f / 256.f, 70.f / 256.f);
}