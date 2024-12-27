#include <pch.h>

#include "DemoApp.h"
#include "BasicCamera.h"
#include "Shader.h"

#pragma comment (lib, "glfw3dll.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "OpenGL32.lib")

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void CursorPosCallback(GLFWwindow* window, double x, double y);
static void MouseButtonCallback(GLFWwindow* window, int Button, int Action, int Mode);


DemoApp* g_App;

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   if (g_App != NULL)
      g_App->KeyboardCB(key, action);
}


static void CursorPosCallback(GLFWwindow* window, double x, double y)
{
   if (g_App != NULL)
      g_App->PassiveMouseCB(x, y);
}


static void MouseButtonCallback(GLFWwindow* window, int Button, int Action, int Mode)
{
   double x, y;

   glfwGetCursorPos(window, &x, &y);

   if (g_App != NULL)
      g_App->MouseCB(Button, Action, (int)x, (int)y);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
   // make sure the viewport matches the new window dimensions; note that width and 
   // height will be significantly larger than specified on retina displays.
   g_App->Reshape(width, height);
}

DemoApp::DemoApp(int windowWidth, int windowHeight, const char* windowTitle, const char* shaderFilePath)
{
   Init(windowWidth, windowHeight, windowTitle);
   InitShader(shaderFilePath);
}
DemoApp::~DemoApp()
{
   Cleanup();
}

void DemoApp::Cleanup()
{
   SAFE_DELETE(m_pBasicCamera);
   SAFE_DELETE(m_pShader);
}

void DemoApp::Run()
{
   while (!glfwWindowShouldClose(m_pWindow)) {
      // per-frame time logic
      double currentFrame = glfwGetTime();
      m_deltaTime = currentFrame - m_lastFrame;
      m_lastFrame = currentFrame;

      RenderScene();

      glfwSwapBuffers(m_pWindow);
      glfwPollEvents();
   }
}

void DemoApp::RenderScene()
{
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   const glm::mat4 viewProj = m_pBasicCamera->GetViewProjMatrix();

   m_pShader->use();
   m_pShader->setMat4("ViewProj", viewProj);
}

void DemoApp::PassiveMouseCB(double x, double y)
{
   m_pBasicCamera->MouseControl((float)x, (float)y);
}

void DemoApp::KeyboardCB(int key, int state)
{
   if (state == GLFW_PRESS) {
      switch (key) {
      case GLFW_KEY_ESCAPE:
      case GLFW_KEY_Q:
         glfwDestroyWindow(m_pWindow);
         glfwTerminate();
         exit(0);

      case GLFW_KEY_W:
         m_isWireframe = !m_isWireframe;

         if (m_isWireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
         }
         else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
         }
         break;
      case GLFW_KEY_UP:
         m_pBasicCamera->ProcessKeyboard(FORWARD, (float)m_deltaTime);
         break;
      case GLFW_KEY_DOWN:
         m_pBasicCamera->ProcessKeyboard(BACKWARD, (float)m_deltaTime);
         break;
      case GLFW_KEY_LEFT:
         m_pBasicCamera->ProcessKeyboard(LEFT, (float)m_deltaTime);
         break;
      case GLFW_KEY_RIGHT:
         m_pBasicCamera->ProcessKeyboard(RIGHT, (float)m_deltaTime);
         break;
      case GLFW_KEY_PAGE_UP:
         m_pBasicCamera->ProcessKeyboard(UP, (float)m_deltaTime);
         break;
      case GLFW_KEY_PAGE_DOWN:
         m_pBasicCamera->ProcessKeyboard(DOWN, (float)m_deltaTime);
         break;
      case GLFW_KEY_R:
         int width, height;
         glfwGetWindowSize(m_pWindow, &width, &height);
         m_pBasicCamera->Reset(width, height);
         break;
      }
   }
}


void DemoApp::MouseCB(int button, int action, int x, int y)
{
}

void DemoApp::Reshape(int width, int height)
{
   m_pBasicCamera->Reshape(width, height);
}

void DemoApp::Init(int windowWidth, int windowHeight, const char* windowTitle)
{
   CreateGlfwWindow(windowWidth, windowHeight, windowTitle);

   InitCallbacks();

   InitCamera(windowWidth, windowHeight);

   glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

   glFrontFace(GL_CCW);
   glCullFace(GL_BACK);

   glEnable(GL_CULL_FACE);
   glEnable(GL_DEPTH_TEST);
}

void DemoApp::InitShader(const char* shaderFilePath)
{
   std::string stringShaderFilePath = std::string(shaderFilePath);

   std::string vertexPath = stringShaderFilePath + ".vs";
   std::string fragmentPath = stringShaderFilePath + ".fs";

   m_pShader = new Shader(vertexPath.c_str(), fragmentPath.c_str());
}


void DemoApp::CreateGlfwWindow(int windowWidth, int windowHeight, const char* windowTitle)
{
   int major_ver = 0;
   int minor_ver = 0;

   bool is_full_screen = false;
   m_pWindow = glfw_init(major_ver, minor_ver, windowWidth, windowHeight, is_full_screen, "Terrain Rendering - Demo 1");

   glfwSetCursorPos(m_pWindow, windowWidth / 2, windowHeight / 2);
}

void DemoApp::InitCallbacks()
{
   glfwSetKeyCallback(m_pWindow, KeyCallback);
   glfwSetCursorPosCallback(m_pWindow, CursorPosCallback);
   glfwSetMouseButtonCallback(m_pWindow, MouseButtonCallback);
   glfwSetFramebufferSizeCallback(m_pWindow, framebuffer_size_callback);
}

void DemoApp::InitCamera(int windowWidth, int windowHeight)
{
   m_pBasicCamera = new BasicCamera(windowWidth, windowHeight, glm::vec3(512.0, 500.0, 512.0));
}