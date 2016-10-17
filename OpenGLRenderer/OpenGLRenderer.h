#pragma once

#include "../Base/BaseRenderer.h"
#include "../Base/include/sdl/SDL.h"
#include "include\glm\glm.hpp"
#include "include\glm\gtc\type_ptr.hpp"
#include "include\glm\gtc\matrix_transform.hpp"

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

class Renderer : public BaseRenderer
{
	void Init();
	void Render(); 
	void SetRenderTarget();
	void Draw();
	void Cleanup();
	void SwapWindow();
	void SetClearColor(float r, float g, float b, float a);
	void ClearScreen();
	int CreateProgram();
	int CreateVertexShaderWithPath(char* path);
	int CreateVertexShaderWithSource(char** source);
	int CreatePixelShaderWithPath(char* path);
	int CreatePixelShaderWithSource(char** source);
	void CompileShader(int shader);
	void AttachShaderToProgram(int shader, int program);
	void LinkProgram(int program);

	int CreateGenericBuffer();
	void FillVBO(int vbo, float* source, int elementNumber);
	int CreateVBO(float* source, int elementNumber);
	void FillIBO(int ibo, int* source, int elementNumber);
	int CreateIBO(int* source, int elementNumber);

	void UseShaderProgram(int program);
	void BindVertexInput(char* variable, int vbo, int sizePerAttr);
	void BindIBO(int ibo);
	void BindUniformMat4f(char* variable, float* value, bool transpose);
	void DrawTriangleFan(int elementNumber);
	void DrawLine(int elementNumber);

	
private:
	SDL_GLContext context;
	int currentShaderProgram = 0;
};
