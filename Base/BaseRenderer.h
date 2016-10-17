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
	virtual void Render() = 0;
	virtual void SwapWindow() = 0;
	virtual int CreateProgram() = 0;
	virtual int CreateVertexShaderWithPath(char* path) = 0;
	virtual int CreateVertexShaderWithSource(char** source) = 0;
	virtual int CreatePixelShaderWithPath(char* path) = 0;
	virtual int CreatePixelShaderWithSource(char** source) = 0;
	virtual void CompileShader(int shader) = 0;
	virtual void AttachShaderToProgram(int shader, int program) = 0;
	virtual void LinkProgram(int program) = 0;

	virtual int CreateGenericBuffer() = 0;
	virtual void FillVBO(int vbo, float* source, int elementNumber) = 0;
	virtual int CreateVBO(float* source, int elementNumber) = 0;
	virtual void FillIBO(int ibo, int* source, int elementNumber) = 0;
	virtual int CreateIBO(int* source, int elementNumber) = 0;

	virtual void UseShaderProgram(int program) = 0;
	virtual void BindVertexInput(char* variable, int vbo, int sizePerAttr) = 0;
	virtual void BindIBO(int ibo) = 0;
	virtual void BindUniformMat4f(char* variable, float* value, bool transpose) = 0;
	virtual void DrawTriangleFan(int elementNumber) = 0;
	virtual void DrawLine(int elementNumber) = 0;
};