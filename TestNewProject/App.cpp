// TestNewProject.cpp : Defines the entry point for the console application.
//
#include "../Base/BaseApp.h"
#include "../OpenGLRenderer/OpenGLRenderer.h"
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <unordered_map>
#include <iostream>
BaseRenderer* renderer = new Renderer();
struct SkeletonNode
{
	aiNode* node;
	std::vector<SkeletonNode*> children;
};
class App: public BaseApp
{
private:
	float vertexData[8] = {-0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f};
	int indexData[4] = { 0,1,2,3 };
	char* vertexShaderSource[1];
	char* pixelShaderSource[1];
	int vbo = 0;
	int ibo = 0;
	int vertexShader = 0;
	int pixelShader = 0;
	int shaderProgram = 0;
	glm::mat4 MVP;
	SkeletonNode* skeleton;

	SkeletonNode* importFBX()
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile("Tad_2013.fbx", aiProcess_CalcTangentSpace);
		if (!scene)
		{
			std::cout << importer.GetErrorString() << std::endl;
		}
		int NumBones = scene->mMeshes[0]->mNumBones;
		
		std::unordered_map<aiNode*, bool> nodeMap;
		aiNode* rootNode = scene->mRootNode;
		nodeMap[rootNode] = true;
		for (int i = 0; i < NumBones; ++i)
		{
			aiBone* bone = scene->mMeshes[0]->mBones[i];
			aiString name = bone->mName;
			aiNode* node = rootNode->FindNode(name);
			aiNode* currentNode = node;
			while (node != rootNode)
			{
				nodeMap[node] = true;
				node = node->mParent;
			}
		}
		SkeletonNode* skeleton = new SkeletonNode;
		BuildSkeleton(skeleton, rootNode, nodeMap);
		return skeleton->children[0];
	}

	void BuildSkeleton(SkeletonNode* parent, aiNode* currentNode, std::unordered_map<aiNode*, bool> nodeMap)
	{
		for (int i = 0; i < currentNode->mNumChildren; ++i)
		{
			aiNode* child = currentNode->mChildren[i];
			if (nodeMap[child] == true)
			{
				SkeletonNode* newNode = new SkeletonNode;
				newNode->node = child;
				parent->children.push_back(newNode);
				BuildSkeleton(newNode, child, nodeMap);
			}
		}
	}
public:
	void Setup()
	{
		skeleton = importFBX();
		vbo = renderer->CreateVBO(vertexData, 8);
		ibo = renderer->CreateIBO(indexData, 4);
		vertexShaderSource[0] =
			"#version 140 \n in vec2 pos2; uniform mat4 MVP; void main() {gl_Position = MVP*vec4(pos2.x, pos2.y, 0, 1);}";
		vertexShader = renderer->CreateVertexShaderWithSource(vertexShaderSource);
		renderer->CompileShader(vertexShader);
		pixelShaderSource[0] =
			"#version 140 \n out vec4 Frag; void main() {Frag = vec4(1.0, 1.0, 1.0, 1.0);}";
		pixelShader = renderer->CreatePixelShaderWithSource(pixelShaderSource);
		renderer->CompileShader(pixelShader);
		shaderProgram = renderer->CreateProgram();
		renderer->AttachShaderToProgram(vertexShader, shaderProgram);
		renderer->AttachShaderToProgram(pixelShader, shaderProgram);
		renderer->LinkProgram(shaderProgram);

		importFBX();
		MVP = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 1.0f, -1.0f) * glm::lookAt(glm::vec3(1.0), glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0));
	}

	void Render()
	{
		renderer->SetClearColor(1.0f, 0.0, 0.0, 1.0);
		renderer->ClearScreen();

		renderer->UseShaderProgram(shaderProgram);
		renderer->BindUniformMat4f("MVP",glm::value_ptr(MVP), true);
		renderer->BindVertexInput("pos2", vbo, 2);
		renderer->BindIBO(ibo);
		//renderer->DrawLine(4);
		renderer->DrawTriangleFan(4);
	}
};

BaseApp* app = new App();