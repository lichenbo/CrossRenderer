#include "BaseApp.h"
#include "base.h"

BaseApp::BaseApp()
{
	
}


BaseApp::~BaseApp()
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

void BaseApp::Cleanup()
{
	Base::Quit();
}
