#pragma once
/// Main Scene Light
///
/// The Directional Light class 
///
/// This class creates the main light used in the scene
#include "Light.h"

class DirectionalLight :
	public Light
{
public:
	DirectionalLight();
	DirectionalLight(GLfloat shadowWidth, GLfloat shadowHeight,
		GLfloat red, GLfloat green, GLfloat blue,
		GLfloat aIntensity, GLfloat dIntensity,
		GLfloat xDir, GLfloat yDir, GLfloat zDir);

	void UseLight(GLfloat ambientIntensityLocation, GLfloat ambientColourLocation,
		GLfloat diffuseIntensityLocation, GLfloat dirLocation);

	glm::mat4 CalculateLightTrans();

	~DirectionalLight();

private:
	glm::vec3 dir;
};
