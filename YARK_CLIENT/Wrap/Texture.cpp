#include "Texture.h"
#include "../lodepng/lodepng.h"
#include <iostream>

GLuint loadTexture(std::string filename, bool linear) {
	std::vector<unsigned char> image;
	unsigned int width, height;
	if (lodepng::decode(image, width, height, filename)) {
		std::cout << "Could not load image: " + filename <<"\n";
		return 0;
	}
	GLuint textureID;
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);

	if (linear) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//GL_LINEAR //GL_NEAREST_MIPMAP_NEAREST
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//GL_LINEAR //GL_NEAREST_MIPMAP_NEAREST
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //GL_NEAREST
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data());
	return textureID;
}