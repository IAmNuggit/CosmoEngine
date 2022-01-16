#define STB_IMAGE_IMPLEMENTATION


/// The Main class contains the main loop for the program 
///
/// This main class brings together all of the components from other  classes to create a game world. 

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <chrono>
#include <Windows.h>
//Audio System
#include <conio.h>
#include <irrKlang.h>

using namespace irrklang;
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

//Input/ Output files
#include <iostream>
#include <fstream>

//GLEW lib
#include <GL\glew.h>
//GLFW lib
#include <GLFW\glfw3.h>

//ASSIMP lib
#include <assimp/Importer.hpp>
//GLM lib
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

//////Header files 
#include "..\GameEngine\CommonValues.h"
#include "..\GameEngine\Window.h"
#include "..\GameEngine\Mesh.h"
#include "..\GameEngine\Shader.h"
#include "..\GameEngine\Camera.h"
#include "..\GameEngine\Texture.h"
#include "..\GameEngine\DirectionalLight.h"
#include "..\GameEngine\PointLight.h"
#include "..\GameEngine\SpotLight.h"
#include "..\GameEngine\Material.h"
#include "..\GameEngine\Model.h"
#include "..\GameEngine\skyBox.h"


GLuint globalProjection = 0, globalModel = 0, globalView = 0, globalEyePosition = 0,
globalSpecularIntensity = 0, globalShininess = 0, globalOmniLightPos = 0, globalFarPlane = 0;
//Window
Window currentWindow;
//Lists
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
//Shader
Shader directShadowShader;
Shader omniDirectShadowShader;
//Camera
Camera playerCamera;
//Texture
Texture FloorTex;
//Material
Material material;
//Models
Model Car;
Model Aircraft;
Model Building;

//Model matrices
glm::mat4 FloorMatrix;
glm::mat4 AircraftMatrix;
glm::mat4 CarMatrix;
glm::mat4 BuildingMatrix;

//Skybox
Skybox skybox;

//Lights
DirectionalLight sceneLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

//Variables
const float toRad = 3.14159265f / 180.0f;
float F1Angle = 0.0f;
float AircraftAngle = 0.0f;
unsigned int pointLightCount = 0;
unsigned int spotLightCount = 0;

//Time
GLfloat updateTime = 0.0f;
GLfloat lastTime = 0.0f;


// Vertex Shader filepath
static const char* vertexShader = "Shaders/shader.vert";

// Fragment Shader filepath
static const char* fragmentShader = "Shaders/shader.frag";


///Brief desc.
///
///This function contains omni and directional fragment and vertex shader file locations 
void CreateShaders()
{
	Shader *shadermain = new Shader();
	shadermain->CreateFromFiles(vertexShader, fragmentShader);
	shaderList.push_back(*shadermain);

	directShadowShader = Shader();
	directShadowShader.CreateFromFiles("Shaders/directional_shadow_map.vert", "Shaders/directional_shadow_map.frag");
	omniDirectShadowShader = Shader();
	omniDirectShadowShader.CreateFromFiles("Shaders/omni_directional_shadow_map.vert", "Shaders/omni_directional_shadow_map.geom", "Shaders/omni_directional_shadow_map.frag");
}

///Brief desc.
///
///This function contains all the model information needed to create models to the scene, including what position in the scene they have and the rotation of the model
void RenderScene()
{
	//Floor Model
	FloorMatrix = glm::mat4(1.0f);
	FloorMatrix = glm::translate(FloorMatrix, glm::vec3(0.0f, -3.0f, 0.0f));
	FloorMatrix = glm::scale(FloorMatrix, glm::vec3(5.0f, 5.0f, 5.0f));
	glUniformMatrix4fv(globalModel, 1, GL_FALSE, glm::value_ptr(FloorMatrix));
	FloorTex.UseTexture();
	material.UseMaterial(globalSpecularIntensity, globalShininess);
	meshList[2]->RenderMesh();

	//Slowly rotates model, once angle reaches 360 reset it to 0. Prevents the variable value getting too high
	AircraftAngle += 0.025f;
	if (AircraftAngle > 360.0f)
	{
		AircraftAngle = 0.1f;
	}
	F1Angle += 0.01f;
	if (F1Angle >= 360)
	{
		F1Angle -= 360;
	}

	//Aircraft Model
	AircraftMatrix = glm::mat4(1.0f);
	AircraftMatrix = glm::rotate(AircraftMatrix, -AircraftAngle * toRad, glm::vec3(0.0f, 1.0f, 0.0f));
	AircraftMatrix = glm::translate(AircraftMatrix, glm::vec3(-60.0f, 40.0f, 10.0f));
	AircraftMatrix = glm::rotate(AircraftMatrix, 160.0f * toRad, glm::vec3(0.0f, 0.0f, 1.0f));
	AircraftMatrix = glm::rotate(AircraftMatrix, -180.0f * toRad, glm::vec3(1.0f, 0.0f, 0.0f));
	AircraftMatrix = glm::scale(AircraftMatrix, glm::vec3(1.5f, 1.5f, 1.5f));
	glUniformMatrix4fv(globalModel, 1, GL_FALSE, glm::value_ptr(AircraftMatrix));
	material.UseMaterial(globalSpecularIntensity, globalShininess);
	Aircraft.RenderModel();

	//Car Model
	CarMatrix = glm::mat4(1.0f);
	CarMatrix = glm::translate(CarMatrix, glm::vec3(-41.0f, 1.2f, -10.0f));
	CarMatrix = glm::rotate(CarMatrix, 30.0f, glm::vec3(0.0f, 0.2f, 0.0f));
	CarMatrix = glm::scale(CarMatrix, glm::vec3(1.4f, 1.4f, 1.4f));
	glUniformMatrix4fv(globalModel, 1, GL_FALSE, glm::value_ptr(CarMatrix));
	material.UseMaterial(globalSpecularIntensity, globalShininess);
	Car.RenderModel();

	//Building Model
	BuildingMatrix = glm::mat4(1.0f);
	BuildingMatrix = glm::translate(BuildingMatrix, glm::vec3(0.0001f, -1.9f, 0.0001f));
	//model = glm::rotate(model, F1Angle * toRad, glm::vec3(0.0f, 0.2f, 0.0f));
	BuildingMatrix = glm::scale(BuildingMatrix, glm::vec3(1.5f, 1.5f, 1.5f));
	glUniformMatrix4fv(globalModel, 1, GL_FALSE, glm::value_ptr(BuildingMatrix));
	material.UseMaterial(globalSpecularIntensity, globalShininess);
	Building.RenderModel();
}

///Brief desc.
///
///This function detects and resolves collisions between the plane and the point
void collidePlane(glm::mat4 matrix, glm::vec3 planePosition, glm::vec3 planeNormal, float border, glm::vec3& point)
{
	// Transform the plane position to world space
	planePosition = matrix * glm::vec4(planePosition, 1.0f);

	// Transform the plane normal to world space
	planeNormal = matrix * glm::vec4(planeNormal, 0.0f);
	planeNormal = glm::normalize(planeNormal);

	// Move the plane along the normal to avoid getting too close
	planePosition += planeNormal * border;

	// Distance from the point to plane
	float d = glm::dot(planePosition - point, planeNormal);

	// When the point is inside, move it to the plane
	if (d > 0)
		point += planeNormal * d;
}

///Brief desc.
///
///This function detects and resolves collisions between the bounding box and the point
void collideBox(glm::mat4 matrix, BoundingBox bbox, float border, glm::vec3& point)
{
	// Reset the nearest distance to the box boundary and the corresponding correction vector
	float dmin = 1e10f;
	glm::vec3 shift(0.0f);

	// Check each side of the bounding box
	for (int i = 0; i < bbox.GetNumPlanes(); i++)
	{
		// Transform the plane position to world space
		glm::vec3 planePosition = bbox.GetPlanePosition(i);
		planePosition = matrix * glm::vec4(planePosition, 1.0f);

		// Transform the plane normal to world space
		glm::vec3 planeNormal = bbox.GetPlaneNormal(i);
		planeNormal = matrix * glm::vec4(planeNormal, 0.0f);
		planeNormal = glm::normalize(planeNormal);

		// Move the plane along the normal to avoid getting too close
		planePosition += planeNormal * border;

		// Distance from the point to plane
		float d = glm::dot(planePosition - point, planeNormal);

		// When the point is outside, there is no collision
		if (d <= 0)
			return;

		// When the point is inside, look for the nearest distance to the box boundary
		if (d < dmin)
		{
			// Update the nearest distance
			dmin = d;

			// Correction vector for moving to the box boundary
			shift = planeNormal * dmin;
		}
	}

	// Move the point to the nearest boundary
	point += shift;
}

///Brief desc.
///
///This function detects and resolves collisions between the camera and objects in the scene
void detectCollisions()
{
	// Get camera position
	glm::vec3 pos = playerCamera.getCameraPos();

	// Minimum allowed distance from the camera to the objects to avoid getting too close
	const float border = 0.2f;

	// Detect and resolve collisions with the floor
	glm::vec3 position(0.0f, 0.0f, 0.0f);
	glm::vec3 normal(0.0f, 1.0f, 0.0f);
	collidePlane(FloorMatrix, position, normal, border, pos);

	// Detect and resolve collisions with each model
	collideBox(AircraftMatrix, Aircraft.GetBoundingBox(), border, pos);
	collideBox(CarMatrix, Car.GetBoundingBox(), border, pos);
	//collideBox(BuildingMatrix, Building.GetBoundingBox(), border, pos);

	// Update camera position
	playerCamera.setCameraPos(pos);
}

void calcAverageNormal(unsigned int * indices, unsigned int indiceCount, GLfloat * vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

///Brief desc.
///
///
void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//		Positions				Colours			  Normals
			-1.0f, -1.0f, -0.6f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		//		Positions		 Colours			  Normals
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	calcAverageNormal(indices, 12, vertices, 32, 8, 5);

	//Create mesh as a pointer so when the function is unused it is not cleared by the clear mesh function
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	//Adds new element at the end of the vector
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);
}

///Brief desc.
///
///This function deals with the basic shadow map 
///@param Main scene light input
void DirectShadowMapPass(DirectionalLight* light)
{
	//Setup our viewport to the same dimensions as the frame buffer
	glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

	light->GetShadowMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	//Multiply projection by the view
	directShadowShader.UseShader();
	globalModel = directShadowShader.GetModelLocation();
	directShadowShader.SetDirectionalLightTransform(&light->CalculateLightTrans());

	directShadowShader.Validate();
	RenderScene();
	//Not really neccesary
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

///Brief desc.
///
///This function deals with the omnishadow map
void OmniDirectShadowMapPass(PointLight* light)
{
	glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

	omniDirectShadowShader.UseShader();
	globalModel = omniDirectShadowShader.GetModelLocation();
	globalOmniLightPos = omniDirectShadowShader.GetOmniLightPosLocation();
	globalFarPlane = omniDirectShadowShader.GetFarPlaneLocation();

	light->GetShadowMap()->Write();

	glClear(GL_DEPTH_BUFFER_BIT);

	glUniform3f(globalOmniLightPos, light->GetPosition().x, light->GetPosition().y, light->GetPosition().z);
	glUniform1f(globalFarPlane, light->GetFarPlane());
	omniDirectShadowShader.SetOmniLightMatrices(light->CalculateLightTransform());

	omniDirectShadowShader.Validate();
	RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPass(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{
	//Viewing size within window
	glViewport(0, 0, 1920, 1080);

	// Clear the window
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//skybox
	skybox.drawSkyBox(viewMatrix, projectionMatrix);
	//Make sure we are using the correct shader
	shaderList[0].UseShader();

	globalModel = shaderList[0].GetModelLocation();
	globalProjection = shaderList[0].GetProjectionLocation();
	globalView = shaderList[0].GetViewLocation();
	globalEyePosition = shaderList[0].GetEyePositionLocation();
	globalSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
	globalShininess = shaderList[0].GetShininessLocation();

	glUniformMatrix4fv(globalProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(globalView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniform3f(globalEyePosition, playerCamera.getCameraPos().x, playerCamera.getCameraPos().y, playerCamera.getCameraPos().z);

	//Go to read (ShadowMap.cpp), set as active texture + bind shadowmap to texture2  
	sceneLight.GetShadowMap()->Read(GL_TEXTURE2);
	//By default is set to zero - For completion sake
	shaderList[0].SetTexture(1);
	shaderList[0].SetDirectionalShadowMap(2);

	glm::vec3 lowerLight = playerCamera.getCameraPos();
	lowerLight.y -= 0.3f;
	spotLights[0].SetFlash(lowerLight, playerCamera.getCameraDir());

	shaderList[0].SetDirectionalLight(&sceneLight);
	shaderList[0].SetPointLights(pointLights, pointLightCount, 3, 0);
	shaderList[0].SetSpotLights(spotLights, spotLightCount, 3 + pointLightCount, pointLightCount);
	shaderList[0].SetDirectionalLightTransform(&sceneLight.CalculateLightTrans());

	shaderList[0].Validate();
	RenderScene();
}


///Brief desc.
///
///This is the main game loop which controls the program. Features of the function include: Window dimensions - Camera information - Texture and model file paths - Lighting information
///Also contains a while loops which loops until the window is closed.
int main()
{
	// Get time
	std::chrono::steady_clock::time_point time1 =
		std::chrono::high_resolution_clock::now();
	//1366, 768
	//initialise window
	currentWindow = Window(1920, 1080); // 1280, 1024 or 1024, 768
	currentWindow.Initialise();


	CreateObjects();
	CreateShaders();

	//Setup starting postions of the camera - Position - pitch/yaw - Camera direction
	playerCamera = Camera(glm::vec3(-30.0f, 10.0f, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -40.0f, 5.0f, 0.5f);

	//Texture file locations
	FloorTex = Texture((char*)"Textures/Floor.png");
	FloorTex.LoadTextureA();

	material = Material(4.0f, 256);

	//Model file locations
	Car = Model();
	Car.LoadModel("Models/Renault12TL.obj");

	Aircraft = Model();
	Aircraft.LoadModel("Models/Intergalactic_Spaceship-(Wavefront).obj");

	Building = Model();
	Building.LoadModel("Models/Map_v1.obj");

	std::vector<std::string> skyboxF;
	skyboxF.push_back("Textures/SkyMap/starfield_rt.tga");
	skyboxF.push_back("Textures/SkyMap/starfield_lt.tga");
	skyboxF.push_back("Textures/SkyMap/starfield_up.tga");
	skyboxF.push_back("Textures/SkyMap/starfield_dn.tga");
	skyboxF.push_back("Textures/SkyMap/starfield_bk.tga");
	skyboxF.push_back("Textures/SkyMap/starfield_ft.tga");

	skybox = Skybox(skyboxF);



	///////////////////////////////////////////////////////////////////////////////
	//////////////////////          Lighting            ///////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	//colour - Intensity - Direction
	sceneLight = DirectionalLight(2048, 2048,
		1.1f, 1.0f, 1.0f,
		0.2f, 0.2f,
		2.0f, 1.0f, -10.0f);

	////////////////Point Lights///////////////////

	//Shadow - colour - Intensity - Position
	pointLights[0] = PointLight(1024, 1024,
		0.1f, 100.0f,
		4.0f, 2.0f, 0.0f, 0.15f, 0.15f,
		30.0f, 5.0f, -30.0f,
		0.3f, 0.01f, 0.01f);
	pointLightCount++;
	////POINTLIGHT INSIDE F1 CAR////
	//Shadow - colour - Intensity - Position
	pointLights[1] = PointLight(1024, 1024,
		0.1f, 100.0f,
		2.0f, 0.0f, 0.0f, 0.2f, 0.2f,
		0.0f, 7.0f, 0.0f,
		0.3f, 0.01f, 0.01f);
	pointLightCount++;

	//Shadow - colour - Intensity - Position
	pointLights[2] = PointLight(1024, 1024,
		0.1f, 100.0f,
		4.0f, 2.0f, 0.0f, 0.1f, 0.1f,
		-18.0f, 4.0f, 37.0f,
		0.3f, 0.01f, 0.01f);
	pointLightCount++;


	//Shadow4 - colour5 - Position - Direction - Intensity
	spotLights[0] = SpotLight(1024, 1024,
		0.1f, 100.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		-10.8f, -1.5f, -10.7f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;
	spotLights[1] = SpotLight(1024, 1024,
		0.1f, 100.0f,
		1.0f, 1.0f, 1.0f,
		0.1f, 1.0f,
		10.8f, -1.5f, 10.7f,
		-100.0f, 10.0f, -90.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;


	// start the sound engine with default parameters
	ISoundEngine* SoundEngine = createIrrKlangDevice();

	if (!SoundEngine)
		return 0; // error starting up the engine

	irrklang::ISound* music = SoundEngine->play3D("media/Run.mp3",
		vec3df(0, 0, 0), true, false, true);

	glm::mat4 projection = glm::perspective(glm::radians(60.0f), (GLfloat)currentWindow.getBufferWidth() / currentWindow.getBufferHeight(), 0.1f, 100.0f);

	spotLights[0].Toggle();
	std::chrono::steady_clock::time_point time2 =
		std::chrono::high_resolution_clock::now();
	// Loop until window closed
	while (!currentWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		//SDL_GetPerformanceCounter();
		updateTime = now - lastTime; // (now - lastTime)*1000/SDL_GetPerformanceFrequency();
		lastTime = now;
		glm::mat4 model(1.0f);
		bool keys[1024];

		for (size_t i = 0; i < 1024; i++)
		{
			keys[i] = 0;
		}

		//GameController 
		int present = glfwJoystickPresent(GLFW_JOYSTICK_1);

		if (1 == present)
		{
			int axesCount;
			const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);

			int buttonCount;
			const unsigned char *buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);

			if (GLFW_PRESS == buttons[1])
			{
				return 0;
			}
			else if (GLFW_RELEASE == buttons[1])
			{
			}
			//Left Joystick
			if (axes[0] <= -0.7) {
				keys[65] = true;
			}
			if (axes[0] >= 0.7) {
				keys[68] = true;
			}
			if (axes[1] >= 0.7) {
				keys[83] = true;
			}
			if (axes[1] <= -0.7) {
				keys[87] = true;
			}
			//Right Joystick
			if (axes[2] <= -0.7) {
			}
			if (axes[2] >= 0.7) {
			}
			if (axes[3] <= -0.7) {
			}
			if (axes[3] >= 0.7) {
			}
		}


		// Get + Handle User Input
		glfwPollEvents();

		playerCamera.keyControl(currentWindow.getsKeys(), keys, updateTime);
		playerCamera.mouseControl(currentWindow.getXChange(), currentWindow.getYChange());
		detectCollisions();

		//If key L is pressed toggle spotlight
		if (currentWindow.getsKeys()[GLFW_KEY_L])
		{
			spotLights[0].Toggle();
			currentWindow.getsKeys()[GLFW_KEY_L] = false;
		}
		if (currentWindow.getsKeys()[GLFW_KEY_E])
		{
			pointLights[0].Toggle();
			currentWindow.getsKeys()[GLFW_KEY_E] = false;
		}
		//Pause Music
		if (currentWindow.getsKeys()[GLFW_KEY_M])
		{
			music->setIsPaused(true);
			currentWindow.getsKeys()[GLFW_KEY_M] = false;
		}
		//Unpause Music
		if (currentWindow.getsKeys()[GLFW_KEY_N])
		{
			music->setIsPaused(false);
			currentWindow.getsKeys()[GLFW_KEY_N] = false;
		}
		//Call shadowMap passes
		DirectShadowMapPass(&sceneLight);
		for (size_t i = 0; i < pointLightCount; i++)
		{
			OmniDirectShadowMapPass(&pointLights[i]);
		}
		for (size_t i = 0; i < spotLightCount; i++)
		{
			OmniDirectShadowMapPass(&spotLights[i]);
		}
		RenderPass(projection, playerCamera.calculateViewMatrix());

		glUseProgram(0);
		// swap buffers and poll IO events
		currentWindow.swapBuffers();
	}
	SoundEngine->drop(); // delete engine

	std::chrono::milliseconds milliseconds =
		std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1);
	std::cout << "Time taken in milliseconds: " << milliseconds.count() << std::endl;

	std::ofstream myfile("PerformanceTesting.txt");
	if (myfile.is_open())
	{
		myfile << "Performance Testing Document\n \n";
		myfile << "Time Taken to start program: " << milliseconds.count() << "(Milliseconds)";
		myfile.close();
	}
	return 0;
}
