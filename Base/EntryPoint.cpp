#include "BaseApp.h"
#include "BaseRenderer.h"

extern BaseApp* app;
extern BaseRenderer* renderer;
int main()
{
	app->Init(renderer);
	app->Setup();
	app->Loop(renderer);
	
}