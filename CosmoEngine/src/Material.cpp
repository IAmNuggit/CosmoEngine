#include "Material.h"

/// The Material class 
///
/// This is a base class used by subclasses such as model

///Brief desc.
///
///**Constructor** - 
///Sets starting values for variables used in the class
Material::Material()
{
	specularIntensity = 0.0f;
	shininess = 0.0f;
}
void Material::UseMaterial(GLuint specularIntensityLocation, GLuint shininessLocation)
{
	glUniform1f(specularIntensityLocation, specularIntensity);
	glUniform1f(shininessLocation, shininess);
}

Material::Material(GLfloat sIntensity, GLfloat shine)
{
	specularIntensity = sIntensity;
	shininess = shine;
}

///Destructor
Material::~Material()
{
}
