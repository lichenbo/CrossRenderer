// TestNewProject.cpp : Defines the entry point for the console application.
//
#include "../Base/BaseApp.h"
#include "../OpenGLRenderer/OpenGLRenderer.h"


class App: public BaseApp
{

};

BaseApp* app = new App();
BaseRenderer* renderer = new Renderer();