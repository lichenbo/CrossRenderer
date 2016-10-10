#pragma once
#include "BaseRenderer.h"
class BaseApp
{
public:
	BaseApp();
	virtual ~BaseApp();
	virtual void Update();
	virtual void Init(BaseRenderer* renderer);
	virtual void Cleanup();
private:
	
};

