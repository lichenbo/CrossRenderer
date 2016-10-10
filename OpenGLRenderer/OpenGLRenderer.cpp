#include "OpenGLRenderer.h"
#include <SDL.h>
#include <GL\glew.h>
#include <SDL_opengl.h>
#include <GL\GLU.h>
#include <iostream>


void Renderer::Init()
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
}

void Renderer::SetRenderTarget()
{

}

void Renderer::Draw()
{

}