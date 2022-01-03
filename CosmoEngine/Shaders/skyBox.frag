#version 330

in vec3 TexCoordinates;

out vec4 colour;

uniform samplerCube skybox;

void main()
{
	colour = texture(skybox, TexCoordinates);
}