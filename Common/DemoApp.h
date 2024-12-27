#pragma once

struct GLFWwindow;
class BasicCamera;
class Shader;

class DemoApp
{
public:
   DemoApp(int windowWidth, int windowHeight, const char* windowTitle, const char* shaderFilePath);
   virtual ~DemoApp();

   virtual void Cleanup();

   void Run();

   virtual void RenderScene();

   void PassiveMouseCB(double x, double y);
   void KeyboardCB(int key, int state);
   void MouseCB(int button, int action, int x, int y);
   void Reshape(int width, int height);

   const Shader& GetShader() const
   {
      return *m_pShader;
   }

private:
   void Init(int windowWidth, int windowHeight, const char* windowTitle);
   void InitShader(const char* shaderFilePath);

   void CreateGlfwWindow(int windowWidth, int windowHeight, const char* windowTitle);
   void InitCallbacks();
   void InitCamera(int windowWidth, int windowHeight);


private:
   GLFWwindow* m_pWindow;
   BasicCamera* m_pBasicCamera;
   Shader* m_pShader;

   bool m_isWireframe = false;

   double m_deltaTime = 0.0f;	// time between current frame and last frame
   double m_lastFrame = 0.0f;
};

