
/// The Skybox class creates cubemap which encapsulates the camera
///
/// The following class creates a cubemap which 6 textured faces. This is then places around the camera with no depth value to give the illusion of the skybox being far away

#include <vector>
#include <string>

#include <GL\glew.h>

//GLM lib
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "CommonValues.h"
#include "Shader.h"
#include "Mesh.h"

class Skybox
{
public:
	Skybox();
	~Skybox();

	Skybox(std::vector<std::string> faceLocation);

	void drawSkyBox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);

private:
	Mesh* skyBoxMesh;
	Shader* skyBoxShader;

	GLuint textureID;
	GLuint globalProjection, globalView;
};