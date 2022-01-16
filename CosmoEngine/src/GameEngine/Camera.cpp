#include "Camera.h"

/// The Camera class creates a fly style camera
///
/// The following class creates a camera which allows you to freely move around the scene using both keyboard and mouse inputs.
/// Features include: Camera movement using WASD, Camera direction controlled by mouse with constraints.

///default constructor
Camera::Camera() {}

///Brief desc.
///
///**Constructor** - 
///Sets starting values for variables used in the class
Camera::Camera(glm::vec3 startPos, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMove, GLfloat startTurn)
{
	//Takes values from Main and sets them as a new variable ready to be used
	moveS = startMove;
	turnS = startTurn;
	pos = startPos;
	camUp = startUp;
	yaw = startYaw;
	pitch = startPitch;
	front = glm::vec3(0.0f, 0.0f, -1.0f);

	update();
}

///Brief desc.
///
///Uniform update function which constantly recalculates variables
void Camera::update()
{
	//Calculate the actual direction vector
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, camUp));
	up = glm::normalize(glm::cross(right, front));
}

///Brief desc.
///
///Calculates the view matrix by using glm::lookat. This will apply all the correct rotations and translations that are needed to look at an object from a position
glm::mat4 Camera::calculateViewMatrix()
{
	return glm::lookAt(pos, pos + front, up);
}

///Get function which returns camera direction
///@returns glm::normalize(front)
glm::vec3 Camera::getCameraDir()
{
	return glm::normalize(front);
}

///Get function which returns position value
///@returns position
glm::vec3 Camera::getCameraPos()
{
	return pos;
}

///Set function which updates position value
void Camera::setCameraPos(glm::vec3 newPos)
{
	pos = newPos;
}

///Brief desc.
///
///Manages key presses (WASD) which moves the camera
///This function is called from Main.cpp and is updated until the mainwindow is closed
///@param a Value of key pressed from the user. Is gained from Window::handleKeys()
///@param b Duration of key press, while key is press continue moving camera
void Camera::keyControl(bool* keys, bool* keysJoy, GLfloat updateTime)
{
	GLfloat velocity = (moveS * updateTime) * 3;

	if (keys[GLFW_KEY_W] | keysJoy[87])
	{
		pos += front * velocity;
	}

	if (keys[GLFW_KEY_S] | keysJoy[83])
	{
		pos -= front * velocity;
	}

	if (keys[GLFW_KEY_A] | keysJoy[65])
	{
		pos -= right * velocity;
	}

	if (keys[GLFW_KEY_D] | keysJoy[68])
	{
		pos += right * velocity;
	}
}


///Brief desc.
///
///Manages mouse movement which changes the viewing angle of the camera
///This function is called from Main.cpp and is updated until the mainwindow is closed
///@param a The change in X value from mouse input
///@param b The change in Y value from mouse input
void Camera::mouseControl(GLfloat xChange, GLfloat yChange)
{
	xChange *= turnS;
	yChange *= turnS;

	yaw += xChange;
	pitch += yChange;
	// constraints to the camera so users won't be able to make strange camera movements
	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}

	if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	update();
}

///Destructor
Camera::~Camera()
{
}
