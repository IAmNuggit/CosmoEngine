#include "Light.h"

/// The Light class
///
/// This is a base class used by multiple subclasses. Contains information needed for all subclasses


///Brief desc.
///
///**Constructor** - 
///Sets starting values for variables used in the class
Light::Light()
{
	//Colour default set to white
	colour = glm::vec3(1.0f, 1.0f, 1.0f);
	//Set default intensity
	ambientIntensity = 1.0f;
	diffuseIntensity = 0.0f;
}


///Brief desc.
///
///Initilise basic shadowmap - Setup light values 
Light::Light(GLfloat shadowWidth, GLfloat shadowHeight, GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity, GLfloat dIntensity)
{
	shadowMap = new ShadowMap();
	shadowMap->Init(shadowWidth, shadowHeight);

	//Used by subclasses
	colour = glm::vec3(red, green, blue);
	ambientIntensity = aIntensity;
	diffuseIntensity = dIntensity;
}
///Destructor
Light::~Light()
{
}
