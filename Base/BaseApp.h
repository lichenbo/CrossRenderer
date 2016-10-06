#pragma once
class BaseApp
{
public:
	BaseApp();
	virtual ~BaseApp();
	virtual void Update();
	virtual void Init();
	virtual void Cleanup();
};

