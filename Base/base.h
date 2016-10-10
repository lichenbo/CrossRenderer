#pragma once
#include "SDL.h"
class BaseRenderer;

class Base
{
public:
	static void Init(BaseRenderer* renderer);
	static void Quit();
};
class WindowManager
{
public:
	static void CreateWindow();
	static void SetWindowProperty(char* title, int width, int height);
	static SDL_Window* Window();
private:
	static SDL_Window* gWindow;
	static char* windowTitle;
	static int windowWidth;
	static int windowHeight;
};
