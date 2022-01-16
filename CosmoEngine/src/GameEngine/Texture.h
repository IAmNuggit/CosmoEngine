#pragma once
/// This class takes images from the texture folder and loads it "onto" the models
///
/// Loads .png image files to a texture - Use texture - Clear texture for memory 

#include <GL\glew.h>
#include "CommonValues.h"

class Texture
{
public:
	Texture();
	Texture(const char* fileLoc);

	bool LoadTexture();
	bool LoadTextureA();

	void UseTexture();
	void ClearTexture();

	~Texture();

private:
	GLuint textureID;
	int width, height, bitDepth;

	const char* fileLocation;
};

