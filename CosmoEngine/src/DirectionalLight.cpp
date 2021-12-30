#include "DirectionalLight.h"

/// The Directional Light class 
///
/// This class creates the main light used in the scene


///Brief desc.
///
///**Constructor** - 
///Sets starting values for variables used in the class
DirectionalLight::DirectionalLight() : Light()
{
	dir = glm::vec3(0.0f, -1.0f, 0.0f);
	lightProj = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 20.0f);
}

DirectionalLight::DirectionalLight(GLfloat shadowWidth, GLfloat shadowHeight,
	GLfloat red, GLfloat green, GLfloat blue,
	GLfloat aIntensity, GLfloat dIntensity,
	GLfloat xDir, GLfloat yDir, GLfloat zDir) : Light(shadowWidth, shadowHeight, red, green, blue, aIntensity, dIntensity)
{
	dir = glm::vec3(xDir, yDir, zDir);
	lightProj = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 100.0f);
}

///Brief desc.
///
///Calculates light transform by using glm::lookat, similar to Camera::calculateViewMatrix().
glm::mat4 DirectionalLight::CalculateLightTrans()
{
	return lightProj * glm::lookAt(-dir, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

///Brief desc.
///
///When this function is called, pass in location of all the variables and the function will bind stored variables to the locations
void DirectionalLight::UseLight(GLfloat ambientIntensityLocation, GLfloat ambientColourLocation,
	GLfloat diffuseIntensityLocation, GLfloat dirLocation)
{
	glUniform3f(ambientColourLocation, colour.x, colour.y, colour.z);
	glUniform1f(ambientIntensityLocation, ambientIntensity);

	glUniform3f(dirLocation, dir.x, dir.y, dir.z);
	glUniform1f(diffuseIntensityLocation, diffuseIntensity);
}

///Destructor
DirectionalLight::~DirectionalLight()
{
}
