#include <Windows.h>
#include <locale>
#include <codecvt>

#include <stdlib.h> // necesare pentru citirea shader-elor
#include <stdio.h>
#include <math.h> 

#include <GL/glew.h>

#include <GLM.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include "Camera.h"
#include "Shader.h"
#include "Model.h"
#include "Helicopter.h"

#include "stb_image.h"

#pragma comment (lib, "glfw3dll.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "OpenGL32.lib")

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

GLuint ProjMatrixLocation, ViewMatrixLocation, WorldMatrixLocation;
Camera* pCamera = nullptr;

bool isNight = false;
bool nKeyPressed = false;

void Cleanup()
{
	delete pCamera;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// timing
double deltaTime = 0.0f;	// time between current frame and last frame
double lastFrame = 0.0f;

void PrintCameraPosition() {
	glm::vec3 pos = pCamera->GetPosition();
	std::cout << "Camera position: x=" << pos.x << ", y=" << pos.y << ", z=" << pos.z << ", yaw =" << pCamera->getYaw() << ", pitch =" << pCamera->getPitch() << std::endl;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_N) {
		if (action == GLFW_PRESS && !nKeyPressed) {
			isNight = !isNight;
			nKeyPressed = true;
		}
		else if (action == GLFW_RELEASE) {
			nKeyPressed = false;
		}
	}


	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	bool cameraMoved = false;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraMoved = pCamera->ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraMoved = pCamera->ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraMoved = pCamera->ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraMoved = pCamera->ProcessKeyboard(RIGHT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cameraMoved = pCamera->ProcessKeyboard(UP, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraMoved = pCamera->ProcessKeyboard(DOWN, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		pCamera->RotateHorizontally(-1.f);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		pCamera->RotateHorizontally(1.f);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		pCamera->RotateVertically(-1.f);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		pCamera->RotateVertically(1.f);

	if (cameraMoved && action == GLFW_PRESS || action == GLFW_REPEAT)
		PrintCameraPosition();

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		pCamera->Reset(width, height);

	}
}

unsigned int backgroundVAO, backgroundVBO, backgroundTexture;

void loadBackgroundTexture(const char* filePath) {
	glGenTextures(1, &backgroundTexture);
	glBindTexture(GL_TEXTURE_2D, backgroundTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(filePath, &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load background texture" << std::endl;
	}
	stbi_image_free(data);
}

void setupBackgroundQuad() {
	float vertices[] = {
		// poziții       // coordonate textură
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f
	};
	unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

	glGenVertexArrays(1, &backgroundVAO);
	glGenBuffers(1, &backgroundVBO);
	unsigned int EBO;
	glGenBuffers(1, &EBO);

	glBindVertexArray(backgroundVAO);

	glBindBuffer(GL_ARRAY_BUFFER, backgroundVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void drawBackground(Shader& backgroundShader) {
	glDisable(GL_DEPTH_TEST); // Dezactivează testul de profunzime pentru fundal
	backgroundShader.use();
	glBindVertexArray(backgroundVAO);
	glBindTexture(GL_TEXTURE_2D, backgroundTexture);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glEnable(GL_DEPTH_TEST); // Reactivare test de profunzime
}


void placeTank(Model& model, Shader& shader, glm::vec3 position) {
	glm::mat4 tankModel = glm::scale(glm::mat4(3.0), glm::vec3(1.f));

	shader.setMat4("projection", pCamera->GetProjectionMatrix());
	shader.setMat4("view", pCamera->GetViewMatrix());
	tankModel = glm::translate(tankModel, position);
	shader.setMat4("model", tankModel);

	model.Draw(shader);
}

void placeHelicopter(Helicopter& model, Shader& shader, glm::vec3 position) {
	glm::mat4 helicopterModel = glm::scale(glm::mat4(1.0), glm::vec3(1.f));

	shader.setMat4("projection", pCamera->GetProjectionMatrix());
	shader.setMat4("view", pCamera->GetViewMatrix());
	helicopterModel = glm::translate(helicopterModel, position);
	helicopterModel = glm::rotate(helicopterModel, glm::radians(90.f), glm::vec3(-1.0f, 0.0f, 0.0f));
	model.SetRootTransf(helicopterModel);

	shader.setMat4("model", helicopterModel);

	model.Draw(shader);
}

unsigned int CreateTexture(const std::string& strTexturePath)
{
	unsigned int textureId = -1;

	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data = stbi_load(strTexturePath.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		std::cout << "Failed to load texture: " << strTexturePath << std::endl;
	}
	stbi_image_free(data);

	return textureId;
}

int main()
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Proiect Tancodrom", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	// tell GLFW to capture our mouse
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewInit();

	glEnable(GL_DEPTH_TEST);

	// Create camera
	pCamera = new Camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(-70.1505, 24.0382, -134.207));

	wchar_t buffer[MAX_PATH];
	GetCurrentDirectoryW(MAX_PATH, buffer);

	std::wstring executablePath(buffer);
	std::wstring wscurrentPath = executablePath.substr(0, executablePath.find_last_of(L"\\/"));

	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::string currentPath = converter.to_bytes(wscurrentPath);
	Shader lightingWithTextureShader((currentPath + "\\Shaders\\PhongLightWithTexture.vs").c_str(), (currentPath + "\\Shaders\\PhongLightWithTexture.fs").c_str());

	std::string helicopterObjFileName = (currentPath + "\\Models\\Helicopter\\uh60.dae");
	Helicopter helicopterObjModel1(helicopterObjFileName, false);
	Helicopter helicopterObjModel2(helicopterObjFileName, false);
	Helicopter helicopterObjModel3(helicopterObjFileName, false);
	Helicopter helicopterObjModel4(helicopterObjFileName, false);
	Helicopter helicopterObjModel5(helicopterObjFileName, false);

	std::string tankObjFileName = (currentPath + "\\Models\\Tank\\IS.obj");
	Model tankObjModel1(tankObjFileName, false);
	Model tankObjModel2(tankObjFileName, false);
	Model tankObjModel3(tankObjFileName, false);
	Model tankObjModel4(tankObjFileName, false);
	Model tankObjModel5(tankObjFileName, false);

	std::string grassObjFileName = (currentPath + "\\Models\\Grass\\10450_Rectangular_Grass_Patch_v1_iterations-2.obj");
	Model grassObjModel(grassObjFileName, false);

	Shader backgroundShader((currentPath + "\\Shaders\\Background.vs").c_str(), (currentPath + "\\Shaders\\Background.fs").c_str());
	setupBackgroundQuad();

	glm::vec3 lightPos(8.0f, -2.3f, -20.0f);

	// render loop
	while (!glfwWindowShouldClose(window)) {
		// per-frame time logic
		double currentFrame = glfwGetTime();

		if (!isNight)
			loadBackgroundTexture((currentPath + "\\Textures\\CloudsDay.png").c_str());
		else
			loadBackgroundTexture((currentPath + "\\Textures\\StarsNight.png").c_str());

		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawBackground(backgroundShader);

		lightingWithTextureShader.use();
		if (!isNight)
		{
			lightingWithTextureShader.SetVec3("lightColor", 1.1f, 1.1f, 1.0f);
			lightingWithTextureShader.setFloat("ambientStrength", 0.6f);
			lightingWithTextureShader.setFloat("diffuseStrength", 0.3f);
			lightingWithTextureShader.setFloat("specularStrength", 1.f);
		}
		else
		{

			lightingWithTextureShader.SetVec3("lightColor", 1.0f, 1.0f, 1.3f);
			lightingWithTextureShader.setFloat("ambientStrength", 0.4f);
			lightingWithTextureShader.setFloat("diffuseStrength", 0.1f);
			lightingWithTextureShader.setFloat("specularStrength", 0.3f);
		}
		lightingWithTextureShader.SetVec3("objectColor", 0.5f, 1.0f, 0.31f);
		lightingWithTextureShader.SetVec3("lightPos", lightPos);
		lightingWithTextureShader.SetVec3("viewPos", pCamera->GetPosition());
		lightingWithTextureShader.setInt("texture_diffuse1", 0);

		//---------------------------------------------------------------------------------------------

		placeHelicopter(helicopterObjModel1, lightingWithTextureShader, glm::vec3(-5.0f, 10.0f, 0.0f));
		placeHelicopter(helicopterObjModel2, lightingWithTextureShader, glm::vec3(-25.0f, 10.0f, 0.0f));
		placeHelicopter(helicopterObjModel3, lightingWithTextureShader, glm::vec3(15.0f, 10.0f, 0.0f));
		placeHelicopter(helicopterObjModel4, lightingWithTextureShader, glm::vec3(5.0f, 20.0f, 10.0f));
		placeHelicopter(helicopterObjModel5, lightingWithTextureShader, glm::vec3(-15.0f, 20.0f, 10.0f));

		//---------------------------------------------------------------------------------------------

		placeTank(tankObjModel1, lightingWithTextureShader, glm::vec3(8.0f, -2.3f, -20.0f));
		placeTank(tankObjModel2, lightingWithTextureShader, glm::vec3(-2.0f, -2.3f, -20.0f));
		placeTank(tankObjModel3, lightingWithTextureShader, glm::vec3(-12.0f, -2.5f, -20.0f));
		placeTank(tankObjModel4, lightingWithTextureShader, glm::vec3(3.0f, -2.3f, -30.0f));
		placeTank(tankObjModel5, lightingWithTextureShader, glm::vec3(-8.0f, -2.5f, -30.0f));

		//---------------------------------------------------------------------------------------------

		lightingWithTextureShader.setMat4("projection", pCamera->GetProjectionMatrix());
		lightingWithTextureShader.setMat4("view", pCamera->GetViewMatrix());
		glm::mat4 grassModel = glm::scale(glm::mat4(1.0), glm::vec3(8.0f,1.f, 8.f));
		grassModel = glm::rotate(grassModel, glm::radians(90.f), glm::vec3(-1.0f, 0.0f, 0.0f));;
		grassModel = glm::translate(grassModel, glm::vec3(0.0f, 0.0f, -15.0f));
		lightingWithTextureShader.setMat4("model", grassModel);
		grassObjModel.Draw(lightingWithTextureShader);

		//---------------------------------------------------------------------------------------------


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	Cleanup();


	// glfw: terminate, clearing all previously allocated GLFW resources
	glfwTerminate();
	return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	pCamera->Reshape(width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	pCamera->MouseControl((float)xpos, (float)ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yOffset)
{
	pCamera->ProcessMouseScroll((float)yOffset);
}
