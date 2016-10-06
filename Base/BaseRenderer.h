#pragma once

class BaseRenderer
{
public:
	virtual void Init() = 0;
	virtual void SetRenderTarget() = 0;
	virtual void Draw() = 0;
};