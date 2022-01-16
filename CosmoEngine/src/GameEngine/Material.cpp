#include "Material.h"

/// Material class which controls specular colour and how it is scattered 
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
///Brief desc.
///
/// Specifies the location of the uniform variable to be modified, and modifies it with the new value.
void Material::UseMaterial(GLuint specularIntensityLocation, GLuint shininessLocation)
{
	glUniform1f(specularIntensityLocation, specularIntensity);
	glUniform1f(shininessLocation, shininess);
}
///Brief desc.
///
///Sets the shininess (Impacts the scattering/radius of the specular highlight)
///Sets the specular Intensity (Sets the color of the specular highlight on the surface)
Material::Material(GLfloat sIntensity, GLfloat shine)
{
	specularIntensity = sIntensity;
	shininess = shine;
}

///Destructor
Material::~Material()
{
}
