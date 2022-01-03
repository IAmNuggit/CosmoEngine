#include "Skybox.h"

Skybox::Skybox()
{

}

Skybox::Skybox(std::vector<std::string> faceLocation)
{
	//Setting up the shader using .vert and .frag files
	skyBoxShader = new Shader();
	skyBoxShader->CreateFromFiles("Shaders/skyBox.vert", "Shaders/skyBox.frag");

	globalProjection = skyBoxShader->GetProjectionLocation();
	globalView = skyBoxShader->GetViewLocation();

	//Setting up the texture
	//Create Cube map - Store in textureID
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, bitDepth;
	//CubeMap - Six Faces
	for (size_t i = 0; i < 6; i++)
	{										//Convert to string
		unsigned char *texData = stbi_load(faceLocation[i].c_str(), &width, &height, &bitDepth, 0);
		if (!texData)
		{
			printf("Failed to find: %s\n", faceLocation[i].c_str());
			return;
		}
		//Attach Images to our CubeMap
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
		//Frees up data and allows it to be used by the program again
		stbi_image_free(texData);
	}
	//Information about CubeMap
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	//Setting up the Mesh

	unsigned int skyBoxIndices[] = {
		// Front Image
		0, 1, 2,
		2, 1, 3,
		// Right Image
		2, 3, 5,
		5, 3, 7,
		// Back Image
		5, 7, 4,
		4, 7, 6,
		// Left Image
		4, 6, 0,
		0, 6, 1,
		// Top Image
		4, 0, 5,
		5, 0, 2,
		// Bottom Image
		1, 6, 3,
		3, 6, 7
	};


	float skyboxVertices[] = {
		//CubeMap requires 8 Values for all Corners
		//Back					-UV values		-Lighting
		-1.0f, 1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, -1.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,

		-1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f
	};

	skyBoxMesh = new Mesh();
	//Number of Vertices and Indices
	skyBoxMesh->CreateMesh(skyboxVertices, skyBoxIndices, 64, 36);
}

void Skybox::drawSkyBox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
	viewMatrix = glm::mat4(glm::mat3(viewMatrix));

	//Disable DepthMask - Enables to see through skybox
	glDepthMask(GL_FALSE);

	//Setup and attach values to Shader
	skyBoxShader->UseShader();
	glUniformMatrix4fv(globalProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(globalView, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	//Setup and attach values to Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	//Validation
	skyBoxShader->Validate();
	//Render
	skyBoxMesh->RenderMesh();

	//Enable DepthMask
	glDepthMask(GL_TRUE);
}

Skybox::~Skybox()
{

}