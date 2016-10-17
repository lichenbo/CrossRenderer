#include <GL\glew.h>
#include "OpenGLRenderer.h"
#include "../Base/base.h"
#include <SDL.h>
#include <iostream>


void Renderer::Init()
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	context = SDL_GL_CreateContext(WindowManager::Window());
	GLCheckError();
	if (context == NULL)
	{
		std::cout << "Create context error" << std::endl;
		return;
	}
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error Initializing GLEW" << std::endl;
		return;
	}
	if (SDL_GL_SetSwapInterval(1) < 0)
	{
		std::cout << "Error setting VSync" << std::endl;
	}
}

void Renderer::Render()
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

		SDL_GL_SwapWindow(WindowManager::Window());
	}
}

void Renderer::SetRenderTarget()
{

}

void Renderer::Draw()
{

}

void Renderer::Cleanup()
{
	SDL_GL_DeleteContext(context);
}

void Renderer::SwapWindow()
{
	SDL_GL_SwapWindow(WindowManager::Window());
}

void Renderer::SetClearColor(float r, float g, float b, float a)
{
	GLCheckError();
	glClearColor(r,g,b,a);
}

void Renderer::ClearScreen()
{
	GLCheckError();
	glClear(GL_COLOR_BUFFER_BIT);
}

int Renderer::CreateProgram()
{
	GLCheckError();
	return glCreateProgram();
}

int Renderer::CreateVertexShaderWithPath(char * path)
{
	GLCheckError();
	GLuint shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(shader, 1, (const GLchar* const*)path, NULL);
	return shader;
}

int Renderer::CreateVertexShaderWithSource(char ** source)
{
	GLCheckError();
	GLuint shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(shader, 1, (const GLchar* const*)source, NULL);
	return shader;
}

int Renderer::CreatePixelShaderWithPath(char * path)
{
	GLCheckError();
	GLuint shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(shader, 1, (const GLchar* const*)path, NULL);
	return shader;
}

int Renderer::CreatePixelShaderWithSource(char ** source)
{
	GLCheckError();
	GLuint shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(shader, 1, (const GLchar* const*)source, NULL);
	return shader;
}

void Renderer::CompileShader(int shader)
{
	GLCheckError();
	glCompileShader(shader);
	GLint compileStatus = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus != GL_TRUE)
	{
		int infoLogLength = 0;
		int maxLength = infoLogLength;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		char* infoLog = new char[maxLength];

		glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
		if (infoLogLength > 0)
		{
			std::cout << infoLog << std::endl;
		}

		delete[] infoLog;
	}
}

void Renderer::AttachShaderToProgram(int shader, int program)
{
	glAttachShader(program, shader);
}

void Renderer::LinkProgram(int program)
{
	glLinkProgram(program);
	GLint programSuccess = GL_TRUE;
	glGetProgramiv(program, GL_LINK_STATUS, &programSuccess);
	if (programSuccess != GL_TRUE)
	{
		int infoLogLength = 0;
		int maxLength = infoLogLength;

		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		char* infoLog = new char[maxLength];

		glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
		if (infoLogLength > 0)
		{
			std::cout << infoLog << std::endl;
		}

		delete[] infoLog;
	}
}

int Renderer::CreateGenericBuffer()
{
	GLuint buffer = 0;
	glGenBuffers(1, &buffer);
	return buffer;
}

void Renderer::FillVBO(int vbo, float * source, int elementNumber)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, elementNumber*sizeof(GLfloat), source, GL_STATIC_DRAW);
}

int Renderer::CreateVBO(float * source, int elementNumber)
{
	int buffer = CreateGenericBuffer();
	FillVBO(buffer, source, elementNumber);
	return buffer;
}

void Renderer::FillIBO(int ibo, int * source, int elementNumber)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementNumber*sizeof(GLuint), source, GL_STATIC_DRAW);
}

int Renderer::CreateIBO(int * source, int elementNumber)
{
	int buffer = CreateGenericBuffer();
	FillIBO(buffer, source, elementNumber);
	return buffer;
}

void Renderer::UseShaderProgram(int program)
{
	glUseProgram(program);
	currentShaderProgram = program;
}

void Renderer::BindVertexInput(char * variable, int vbo, int sizePerAttr)
{
	int attribLoc = glGetAttribLocation(currentShaderProgram, variable);
	if (attribLoc < -1)
	{
		std::cout << "Cannot get " << variable << " in shader" << std::endl;
	}
	glEnableVertexAttribArray(attribLoc);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(attribLoc, sizePerAttr, GL_FLOAT, GL_FALSE, 0, NULL);
}

void Renderer::BindIBO(int ibo)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
}

void Renderer::BindUniformMat4f(char * variable, float * value, bool transpose)
{
	int loc = glGetUniformLocation(currentShaderProgram, variable);
	if (loc == -1)
	{
		std::cout << "Not find uniform " << variable << std::endl;
	}
	if (transpose)
		glUniformMatrix4fv(loc, 1, GL_TRUE, value);
	else
		glUniformMatrix4fv(loc, 1, GL_FALSE, value);
}

void Renderer::DrawTriangleFan(int elementNumber)
{
	glDrawElements(GL_TRIANGLE_FAN, elementNumber, GL_UNSIGNED_INT, NULL);
}

void Renderer::DrawLine(int elementNumber)
{
	glDrawElements(GL_LINES, elementNumber, GL_UNSIGNED_INT, NULL);
}
