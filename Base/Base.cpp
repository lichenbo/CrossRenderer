#include "base.h"
#include "SDL.h"
#include <iostream>

void WindowManager::CreateWindow()
{

}

void Base::Init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cout << "SDL INIT ERROR" << std::endl;
	}

}

void Base::Quit()
{
	SDL_Quit();
}
