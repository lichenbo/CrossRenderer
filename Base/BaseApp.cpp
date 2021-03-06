#include "BaseApp.h"
#include "base.h"
#include <chrono>
#include <iostream>
BaseApp::BaseApp()
{
	
}


BaseApp::~BaseApp()
{
}

void BaseApp::Setup()
{
}

void BaseApp::Update()
{
}

void BaseApp::Init(BaseRenderer* renderer)
{
	Base::Init(renderer);
	WindowManager::CreateWindow();
	renderer->Init();
}

void BaseApp::Loop(BaseRenderer * renderer)
{
	SDL_StartTextInput();
	bool quit = false;
	SDL_Event e;
	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			else if (e.type == SDL_TEXTINPUT)
			{
				int x = 0, y = 0;
				SDL_GetMouseState(&x, &y);
				
			}
		}
		typedef std::chrono::high_resolution_clock Time;
		typedef std::chrono::milliseconds ms;
		typedef std::chrono::duration<float> fsec;
		auto t0 = Time::now();

		this->Update();
		this->Render();

		auto t1 = Time::now();
		renderer->SwapWindow();

		fsec fs = t1 - t0;
		ms d = std::chrono::duration_cast<ms>(fs);
		int time_count = d.count();
		if (time_count < 1)
			time_count = 1;
		std::cout << 1000 / time_count << "fps\n";
	}

	SDL_StopTextInput();
}

void BaseApp::Render()
{
}

void BaseApp::Cleanup()
{
	Base::Quit();
}
