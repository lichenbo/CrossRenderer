#pragma once
#include "include/sdl/SDL.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_sdl_gl3.h"
class BaseRenderer;

class Base
{
public:
	static void Init(BaseRenderer* renderer);
	static void Quit();
private:
	static BaseRenderer* renderer;
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
class InputManager
{
public:
	static bool KeyDown(SDL_Scancode code);
};
