#include "NavBall.h"

NavBall::NavBall(Client** client, XY pos, Cam* cam, Window* win,Font* font) {
	this->f = font;
	this->win = win;
	this->cam = cam;
	this->pos = pos;
	this->client = client;

	shader = LoadRawShader("Shaders/sphere.vert", "Shaders/sphere.frag");
	glUseProgram(shader);
	proj = glGetUniformLocation(shader, "proj");
	model = glGetUniformLocation(shader, "model");
	rot = glGetUniformLocation(shader, "rot");
	roll = glGetUniformLocation(shader, "roll");

	GLfloat vertices[6][2] = {
		{-1.f,-1.f},
		{ -1.f,1.f},
		{ 1.f,-1.f},
		{ 1.f,-1.f},
		{ -1.f,1.f},
		{ 1.f,1.f}
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


	navballTex = new Texture("Tex/navball3.png");
	size = XY{ 400,400 };
}

#include "gtc\matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/quaternion.hpp"

void NavBall::Tick(Draw* draw) {
	draw->BindDraw2DShader();
	draw->SetDrawColor2D(0, 1, 0);
	draw->DrawRect2D(pos.x, pos.y, pos.x + size.x, pos.y + size.y);


	DataIn DI = (*client)? (*client)->dataIn : DataIn();

	draw->BindTextShader();
	draw->SetDrawColor2D(0,1,0);
	draw->DrawString(f, std::to_string(DI.Pitch), 10, 30);
	draw->DrawString(f, std::to_string(DI.Heading), 10, 60);
	draw->DrawString(f, std::to_string(DI.Roll), 10, 90);

	cam->Calculate();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, navballTex->textureID);

	float rad = size.x / 2;

	glUseProgram(shader);
	SetShaderMat4(proj, cam->projection);

	glm::mat4 modelMat = glm::mat4(1);
	modelMat = glm::translate(modelMat, glm::vec3(pos.x+size.x/2, pos.y+size.y/2, 0));
	modelMat = glm::scale(modelMat, glm::vec3(rad, rad, rad));
	SetShaderMat4(model, modelMat);



	glm::mat4 rotMat = glm::mat4(1);

	glm::mat4 rotMat2 = glm::mat4(1);
	if (*client) {
		//std::cout << "Roll: " << (*client)->dataIn.t1 << "\n";
	//	rotMat = glm::toMat4(glm::quat((*client)->dataIn.t1, (*client)->dataIn.t2, (*client)->dataIn.t3, (*client)->dataIn.t4));
	
		rotMat2 = glm::rotate(rotMat2, glm::radians(-(*client)->dataIn.Roll), glm::vec3(0,	0, 1));
		
		rotMat = glm::rotate(rotMat, glm::radians(-(*client)->dataIn.Heading + 90), glm::vec3(0, 0, 1));
		rotMat = glm::rotate(rotMat, glm::radians((*client)->dataIn.Pitch-90), glm::vec3(1, 0, 0));
		//glUniform1f(roll, glm::radians(5.f)); //-(*client)->dataIn.Roll

	}
	SetShaderMat4(rot, rotMat);
	SetShaderMat4(roll, rotMat2);
	//glUniformMatrix3fv(unif, 1, false, glm::value_ptr(rotMat2));

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}