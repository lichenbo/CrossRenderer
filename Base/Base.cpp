#include "base.h"
#include "SDL.h"
#include "BaseRenderer.h"
#include <iostream>

SDL_Window* gWindow = NULL;
char* WindowManager::windowTitle = "Default";
int WindowManager::windowWidth = 800;
int WindowManager::windowHeight = 600;

void WindowManager::CreateWindow()
{
	std::cout << "Window size: " << windowWidth << " x " << windowHeight << std::endl;
	gWindow = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (gWindow == NULL)
	{
		std::cout << "Create window Error" << std::endl;
	}
}

void WindowManager::SetWindowProperty(char * title, int width, int height)
{
	windowTitle = title;
	windowWidth = width;
	windowHeight = height;
}

SDL_Window * WindowManager::Window()
{
	if (gWindow == NULL)
	{
		std::cout << "Cannot get window" << std::endl;
	}
	return gWindow;
}

void Base::Init(BaseRenderer* renderer)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cout << "SDL INIT ERROR" << std::endl;
	}

	renderer->Init();

}

void Base::Quit()
{
	SDL_Quit();
}
