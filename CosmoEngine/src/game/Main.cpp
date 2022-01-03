#define STB_IMAGE_IMPLEMENTATION


/// The Main class contains the main loop for the program 
///
/// This main class contains the following features: External filepath locations, model and light information and other functions include a Average normal calculations
#include <iostream>
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
Model Aircraft;
Model F1;
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

// start the sound engine with default parameters
ISoundEngine *SoundEngine = createIrrKlangDevice();

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
	glm::mat4 model(1.0f);

	//Floor Model
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
	model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
	glUniformMatrix4fv(globalModel, 1, GL_FALSE, glm::value_ptr(model));
	FloorTex.UseTexture();
	material.UseMaterial(globalSpecularIntensity, globalShininess);
	meshList[2]->RenderMesh();

	//Slowly rotates model, once angle reaches 360 reset it to 0. Prevents the variable value getting too high
	AircraftAngle += 0.05f;
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
	model = glm::mat4(1.0f);
	model = glm::rotate(model, -AircraftAngle * toRad, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(-25.0f, 0.0f, 10.0f));
	model = glm::rotate(model, 160.0f * toRad, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, -180.0f * toRad, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
	glUniformMatrix4fv(globalModel, 1, GL_FALSE, glm::value_ptr(model));
	material.UseMaterial(globalSpecularIntensity, globalShininess);
	Aircraft.RenderModel();

	//F1 Car Model
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0001f, -1.9f, 0.0001f));
	//model = glm::rotate(model, F1Angle * toRad, glm::vec3(0.0f, 0.2f, 0.0f));
	model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
	glUniformMatrix4fv(globalModel, 1, GL_FALSE, glm::value_ptr(model));
	material.UseMaterial(globalSpecularIntensity, globalShininess);
	F1.RenderModel();

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
	// start the sound engine with default parameters
	ISoundEngine* SoundEngine = createIrrKlangDevice();

	if (!SoundEngine)
		return 0; // error starting up the engine

	ISound* music = SoundEngine->play3D("media/GameMusic.mp3",
		vec3df(0, 0, 0), true, false, true);

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
	playerCamera = Camera(glm::vec3(0.0f, 10.0f, 15.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -40.0f, 5.0f, 0.5f);

	//Texture file locations
	FloorTex = Texture((char*)"Textures/Floor.png");
	FloorTex.LoadTextureA();

	material = Material(4.0f, 256);

	//Model file locations
	Aircraft = Model();
	Aircraft.LoadModel("Models/Intergalactic_Spaceship-(Wavefront).obj");

	F1 = Model();
	F1.LoadModel("Models/Formula 1 mesh.obj");

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
		0.0f, 2.0f, 0.0f, 0.3f, 0.3f,
		30.0f, 5.0f, 0.0f,
		0.3f, 0.01f, 0.01f);
	pointLightCount++;
	////POINTLIGHT INSIDE F1 CAR////
	//Shadow - colour - Intensity - Position
	pointLights[1] = PointLight(1024, 1024,
		0.1f, 100.0f,
		1.0f, 0.0f, 0.0f, 0.0f, 0.4f,
		0.0f, 4.0f, 0.0f,
		0.3f, 0.01f, 0.01f);
	pointLightCount++;

	//Shadow - colour - Intensity - Position
	pointLights[2] = PointLight(1024, 1024,
		0.1f, 100.0f,
		0.0f, 0.0f, 2.0f, 0.3f, 0.3f,
		-30.0f, 4.0f, 0.0f,
		0.3f, 0.01f, 0.01f);
	pointLightCount++;
	//Shadow - colour - Intensity - Position
	pointLights[3] = PointLight(1024, 1024,
		0.1f, 100.0f,
		0.0f, 0.0f, 200.0f, 0.3f, 0.3f,
		20.0f, 4.0f, 0.0f,
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

	glm::mat4 projection = glm::perspective(glm::radians(60.0f), (GLfloat)currentWindow.getBufferWidth() / currentWindow.getBufferHeight(), 0.1f, 100.0f);


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

		//GameController 
		int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
		//std::cout << "GamePad One status: " << present << std::endl;

		if (1 == present)
		{
			int axesCount;
			const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
			//std::cout << "Number of Axes: " << axesCount << std::endl;

			std::cout << "Left Stick X Axis: " << axes[0] << std::endl;
			std::cout << "Left Stick Y Axis: " << axes[1] << std::endl;
			std::cout << "Right Stick X Axis: " << axes[2] << std::endl;
			std::cout << "Right Stick Y Axis: " << axes[3] << std::endl;
			std::cout << "Left Trigger/L1: " << axes[4] << std::endl;
			std::cout << "Right Trigger/R1: " << axes[5] << std::endl;
		}

		// Get + Handle User Input
		glfwPollEvents();

		playerCamera.keyControl(currentWindow.getsKeys(), updateTime);
		playerCamera.mouseControl(currentWindow.getXChange(), currentWindow.getYChange());
<<<<<<< Updated upstream
=======


>>>>>>> Stashed changes
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
