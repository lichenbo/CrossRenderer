#include "BaseApp.h"
#include "base.h"

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
		this->Render();
		renderer->SwapWindow();
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
