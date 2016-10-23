#pragma once
#include "BaseRenderer.h"
class BaseApp
{
public:
	BaseApp();
	virtual ~BaseApp();
	virtual void Setup();
	virtual void Update();
	virtual void Init(BaseRenderer* renderer);
	virtual void Loop(BaseRenderer* renderer);
	virtual void Render();
	virtual void Cleanup();
	
private:
	
};

