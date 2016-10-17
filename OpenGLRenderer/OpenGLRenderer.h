#pragma once

#include "../Base/BaseRenderer.h"
#include "../Base/include/sdl/SDL.h"
#include <assert.h>

#define GLCheckError()\
{\
	GLenum err = glGetError();\
	if (err != GL_NO_ERROR)\
	{\
		for (; err != GL_NO_ERROR; err = glGetError())\
		{\
			assert(false);\
		}\
	}\
}

class Renderer : public BaseRenderer{	void Init();	void Render(); 	void SetRenderTarget();	void Draw();	void Cleanup();	void SwapWindow();	void SetClearColor(float r, float g, float b, float a);	void ClearScreen();	int CreateProgram();	int CreateVertexShaderWithPath(char* path);	int CreateVertexShaderWithSource(char* source);	int CreatePixelShaderWithPath(char* path);	int CreatePixelShaderWithSource(char* source);	void CompileShader(int shader);	void AttachShaderToProgram(int shader, int program);	void LinkProgram(int program);private:	SDL_GLContext context;};