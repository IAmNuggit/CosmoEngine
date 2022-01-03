


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