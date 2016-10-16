#pragma once

class BaseRenderer
{
public:
	virtual void Init() = 0;
	virtual void SetRenderTarget() = 0;
	virtual void Draw() = 0;
	virtual void Cleanup() = 0;
	virtual void SetClearColor(float r, float g, float b, float a) = 0;
	virtual void ClearScreen() = 0;
};