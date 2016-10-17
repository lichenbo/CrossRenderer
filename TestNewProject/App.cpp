// TestNewProject.cpp : Defines the entry point for the console application.
//
#include "../Base/BaseApp.h"
#include "../OpenGLRenderer/OpenGLRenderer.h"


BaseRenderer* renderer = new Renderer();

class App: public BaseApp
{
public:
	void Render()
	{
		renderer->SetClearColor(1.0f, 0.0, 0.0, 1.0);
		renderer->ClearScreen();
	}
};

BaseApp* app = new App();