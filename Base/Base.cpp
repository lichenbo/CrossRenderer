#include "base.h"
#include "SDL.h"
#include "BaseRenderer.h"
#include <iostream>

SDL_Window* WindowManager::gWindow = NULL;
char* WindowManager::windowTitle = "Default";
int WindowManager::windowWidth = 800;
int WindowManager::windowHeight = 600;

BaseRenderer* Base::renderer = NULL;

void WindowManager::CreateWindow()
{
	std::cout << "Window size: " << windowWidth << " x " << windowHeight << std::endl;
	gWindow = SDL_CreateWindow(windowTitle, 100, 100, windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	std::cout << SDL_GetError() << std::endl;
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
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "SDL INIT ERROR" << std::endl;
		return;
	}

	Base::renderer = renderer;
}

void Base::Quit()
{
	SDL_Quit();
}

bool InputManager::KeyDown(SDL_Scancode code)
{
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (state[code]) {
		return true;
	}
	return false;
}
