#pragma once
/// The Camera class HEADER
#include <GL\glew.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <GLFW\glfw3.h>

class Camera
{
public:
	Camera();
	Camera(glm::vec3 startPos, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMove, GLfloat startTurn);

	void keyControl(bool* keys, GLfloat updateTime);
	void mouseControl(GLfloat xChange, GLfloat yChange);

	glm::vec3 getCameraPos();
	glm::vec3 getCameraDir();

	glm::mat4 calculateViewMatrix();

	~Camera();

private:
	glm::vec3 pos;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 camUp;

	GLfloat yaw;
	GLfloat pitch;

	GLfloat moveS;
	GLfloat turnS;

	void update();
};

