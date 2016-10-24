// TestNewProject.cpp : Defines the entry point for the console application.
//
#include "../Base/base.h"
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
	aiNodeAnim* anim;
	std::vector<SkeletonNode*> children;
};

class Quaternion
{
public:
	Quaternion() {}
	Quaternion(float s, float i, float j, float k): s(s), i(i), j(j), k(k) {}
	Quaternion(const aiQuaternion& aiquat):s(aiquat.w), i(aiquat.x), j(aiquat.y), k(aiquat.z)	{}
	Quaternion(const glm::vec4& v) : s(v.w), i(v.x), j(v.y), k(v.z) {}
	Quaternion(float s, const glm::vec3& v) : s(s), i(v.x), j(v.y), k(v.z) {}
	Quaternion(const glm::vec3& axis, float radius)
	{
		s = cos(radius / 2);
		glm::vec3 v = sin(radius / 2) * axis;
		i = v.x;
		j = v.y;
		k = v.z;
	}
	Quaternion(const glm::vec3& v) : Quaternion(0, v) {}
	glm::mat3 matrix() 
	{
		/* Column major matrix */
		glm::vec3 v = glm::vec3(i, j, k);
		glm::mat3 head = glm::mat3(i*i,i*j,i*k,i*j,j*j,j*k,i*k,j*k,k*k);
		glm::mat3 tilda = glm::mat3(0.f, k, -j, -k, 0.f, i, j, -i, 0.f);
	
		return ((s*s - glm::dot(v, v)) * glm::mat3(1.0f) + 2.f * head + 2 * s*tilda);
	}
	Quaternion conjugate()
	{
		return Quaternion(s, -i, -j, -k);
	}
	Quaternion operator+ (const Quaternion& q)
	{
		return Quaternion(s + q.s, i + q.i, j + q.j, k + q.k);
	}
	Quaternion operator- (const Quaternion& q)
	{
		return Quaternion(s - q.s, i - q.i, j - q.j, k - q.k);
	}
	Quaternion operator/ (float f)
	{
		return Quaternion(s / f, i / f, j / f, k / f);
	}
	Quaternion operator* (float f)
	{
		return Quaternion(s*f, i*f, j*f, k*f);
	}
	Quaternion operator* (Quaternion q)
	{
		glm::vec3 v1 = glm::vec3(i, j, k);
		glm::vec3 v2 = glm::vec3(q.i, q.j, q.k);
		return Quaternion(s*q.s - glm::dot(v1, v2), s*v2 + q.s*v1 + glm::cross(v1,v2));
	}
	float dot(Quaternion q)
	{
		glm::vec3 v1 = glm::vec3(i, j, k);
		glm::vec3 v2 = glm::vec3(q.i, q.j, q.k);
		return s*q.s + glm::dot(v1, v2);
	}
	float squareMagnitude()
	{
		return s*s + i*i + j*j + k*k;
	}
	float magnitude()
	{
		return sqrt(squareMagnitude());
	}
	Quaternion inverse()
	{
		return conjugate() / squareMagnitude();
	}
	glm::vec3 rotate(glm::vec3 point)
	{
		Quaternion result = *this * Quaternion(0, point) * inverse();
		return glm::vec3(result.i, result.j, result.k);
	}
private:
	float s, i, j, k;
	
};

class VQM
{
public:
	VQM(glm::vec3 v, const Quaternion& q, glm::vec3 m) : v(v), q(q), m(m) {}
	glm::mat4 matrix()
	{
		return glm::translate(glm::mat4(1.0f), v) * glm::mat4(q.matrix()) * glm::scale(glm::mat4(1.0f), m);
	}
private:
	glm::vec3 v;
	Quaternion q;
	glm::vec3 m;
};
class App: public BaseApp
{
private:
	float vertexData[8] = {-0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f};
	std::vector<float> boneVertexData;
	int indexData[4] = { 0,1,2,3 };
	char* vertexShaderSource[1];
	char* pixelShaderSource[1];
	int vbo = 0;
	int ibo = 0;
	int vertexShader = 0;
	int pixelShader = 0;
	int shaderProgram = 0;
	glm::mat4 MVP;
	glm::vec3 lookAtDir;
	glm::vec3 cameraPos;
	glm::vec3 upVec;
	glm::vec3 leftVec;
	SkeletonNode* skeleton;
	int current_tick = 0;
	int max_tick = 0;

	SkeletonNode* importFBX()
	{
		Assimp::Importer* importer = new Assimp::Importer;
		const aiScene* scene = importer->ReadFile("Tad_2013.fbx", aiProcess_CalcTangentSpace);
		if (!scene)
		{
			std::cout << importer->GetErrorString() << std::endl;
		}
		int NumBones = scene->mMeshes[0]->mNumBones;
		
		std::unordered_map<aiNode*, bool> nodeMap;
		aiNode* rootNode = scene->mRootNode;
		aiAnimation* animation = scene->mAnimations[0];
		max_tick = animation->mDuration;
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
		BuildSkeleton(skeleton, rootNode, nodeMap, animation);
		return skeleton->children[0];
	}

	void BuildSkeleton(SkeletonNode* parent, aiNode* currentNode, std::unordered_map<aiNode*, bool> nodeMap, aiAnimation* animation)
	{
		

		for (int i = 0; i < currentNode->mNumChildren; ++i)
		{
			aiNode* child = currentNode->mChildren[i];
			if (nodeMap[child] == true)
			{
				SkeletonNode* newNode = new SkeletonNode;
				newNode->node = child;
				newNode->anim = NULL;
				for (int i = 0; i < animation->mNumChannels; ++i)
				{
					if (animation->mChannels[i]->mNodeName == child->mName)
					{
						newNode->anim = animation->mChannels[i];
						break;
					}
				}
				parent->children.push_back(newNode);
				BuildSkeleton(newNode, child, nodeMap, animation);
			}
		}
	}
public:
	void Setup()
	{
		BaseApp::Setup();
		vbo = renderer->CreateVBO(vertexData, 8);
		ibo = renderer->CreateIBO(indexData, 4);
		vertexShaderSource[0] =
			"#version 140 \n in vec4 pos4; uniform mat4 MVP; void main() {gl_Position = MVP*pos4;}";
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

		skeleton = importFBX();
		cameraPos = glm::vec3(0.0, 0.0, 10.0);
		lookAtDir = glm::vec3(0.0, 0.0, -1.0);
		upVec = glm::vec3(0.0, 1.0, 0.0);
		leftVec = glm::normalize(glm::cross(lookAtDir, upVec));
		
		MVP = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 100.0f, -100.0f) * glm::lookAt(cameraPos, lookAtDir + cameraPos, upVec);
	}

	
	/* Read skeleton and interpolate the animation at runtime */
	void readSkeletonHelper(SkeletonNode* parent, glm::vec4 parent_pos, glm::mat4 parent_matrix, SkeletonNode* current, int current_tick)
	{	

		glm::mat4 matrix;
		/* if not animation provided, use the default transformation info */
		if (current->anim == NULL || current->anim->mNumPositionKeys == 0)
		{
			matrix = parent_matrix * matrixConverter(current->node->mTransformation);
		}
		else
		{
			/* interpolation with translation */
			glm::vec3 currentPosition;
			/* if only one key frame provided, do not interpolate */
			if (current->anim->mNumPositionKeys == 1)
			{
				currentPosition = vectorConverter(current->anim->mPositionKeys[0].mValue);
			}
			/* interpolate using LERP */
			for (int i = 0; i < current->anim->mNumPositionKeys - 1; ++i)
			{
				if (current_tick >= current->anim->mPositionKeys[i].mTime && current_tick <= current->anim->mPositionKeys[i + 1].mTime)
				{
					currentPosition = vectorConverter((current->anim->mPositionKeys[i + 1].mValue - current->anim->mPositionKeys[i].mValue) / (float)(current->anim->mPositionKeys[i + 1].mTime - current->anim->mPositionKeys[i].mTime) * (float)(current_tick - current->anim->mPositionKeys[i].mTime) + current->anim->mPositionKeys[i].mValue);
				}

			}
			/* interpolation with scaling */
			glm::vec3 currentScaling;
			if (current->anim->mNumScalingKeys == 1)
			{
				currentScaling = vectorConverter(current->anim->mScalingKeys[0].mValue);
			}
			for (int i = 0; i < current->anim->mNumScalingKeys - 1; ++i)
			{
				if (current_tick >= current->anim->mScalingKeys[i].mTime && current_tick <= current->anim->mScalingKeys[i + 1].mTime)
				{
					currentScaling = vectorConverter((current->anim->mScalingKeys[i + 1].mValue - current->anim->mScalingKeys[i].mValue) / (float)(current->anim->mScalingKeys[i + 1].mTime - current->anim->mScalingKeys[i].mTime) * (float)(current_tick - current->anim->mScalingKeys[i].mTime) + current->anim->mScalingKeys[i].mValue);
				}
			}
			/* interpolation with rotation (Quaternion) */
			Quaternion currentRotation;
			if (current->anim->mNumRotationKeys == 1)
			{
				currentRotation = Quaternion(current->anim->mRotationKeys[0].mValue);
			}
			for (int i = 0; i < current->anim->mNumRotationKeys - 1; ++i)
			{
				if (current_tick >= current->anim->mRotationKeys[i].mTime && current_tick <= current->anim->mRotationKeys[i + 1].mTime)
				{
					currentRotation = (Quaternion(current->anim->mRotationKeys[i + 1].mValue) - Quaternion(current->anim->mRotationKeys[i].mValue)) / (float)(current->anim->mRotationKeys[i + 1].mTime - current->anim->mRotationKeys[i].mTime) * (float)(current_tick - current->anim->mRotationKeys[i].mTime) + Quaternion(current->anim->mRotationKeys[i].mValue);
				}
			}

			/* convert Quaternion to matrix */
			matrix = parent_matrix * VQM(currentPosition, currentRotation, currentScaling).matrix();
		}

		/* add the current transformation info to the rendering buffer */
		glm::vec4 current_pos = matrix * glm::vec4(0.0, 0.0, 0.0, 1.0);
		if (parent != NULL)
		{
			for (int i = 0; i < 4; ++i)
				boneVertexData.push_back(parent_pos[i]);
			for (int i = 0; i < 4; ++i)
				boneVertexData.push_back(current_pos[i]);
		}
		/* recursively do the same thing for the children */
		for (SkeletonNode* child : current->children)
		{
			readSkeletonHelper(current, current_pos,matrix, child, current_tick);
		}
	}

	/* convert the assimp matrix to glm::mat4 (column major) */
	glm::mat4 matrixConverter(const aiMatrix4x4& m)
	{
		glm::mat4 matrix;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				matrix[i][j] = m[i][j];
		return glm::transpose(matrix);
	}
	glm::vec3 vectorConverter(const aiVector3t<float>& m)
	{
		return glm::vec3(m.x, m.y, m.z);
	}

	/* Update animation per frame */
	void Update()
	{
		BaseApp::Update();
		float cameraSpeed = 0.4;
		if (InputManager::KeyDown(SDL_SCANCODE_W))
		{
			cameraPos += cameraSpeed*glm::normalize(lookAtDir);
		}
		if (InputManager::KeyDown(SDL_SCANCODE_S))
		{
			cameraPos -= cameraSpeed*glm::normalize(lookAtDir);
		}
		if (InputManager::KeyDown(SDL_SCANCODE_A))
		{
			cameraPos += cameraSpeed*glm::normalize(leftVec);
		}
		if (InputManager::KeyDown(SDL_SCANCODE_D))
		{
			cameraPos -= cameraSpeed*glm::normalize(leftVec);
		}
		if (InputManager::KeyDown(SDL_SCANCODE_LSHIFT))
		{
			cameraPos += cameraSpeed*glm::normalize(upVec);

		}
		if (InputManager::KeyDown(SDL_SCANCODE_LCTRL))
		{
			cameraPos -= cameraSpeed*glm::normalize(upVec);
		}
		

		current_tick++;
		if (current_tick == max_tick)
			current_tick = 0;

		leftVec = glm::normalize(glm::cross(lookAtDir, upVec));
		MVP = glm::perspectiveFov((float)M_PI/2, 100.f,100.f, 0.1f, 100.0f) * glm::lookAt(cameraPos, lookAtDir + cameraPos, upVec);

		boneVertexData.clear();
		readSkeletonHelper(NULL, glm::vec4(0.0,0.0,0.0,1.0), glm::mat4(1.0), skeleton, current_tick);
		renderer->FillVBO(vbo, &boneVertexData[0], boneVertexData.size());
	}

	/* Render content per frame */
	void Render()
	{
		BaseApp::Render();
		renderer->SetClearColor(1.0f, 0.0, 0.0, 1.0);
		renderer->ClearScreen();

		renderer->UseShaderProgram(shaderProgram);
		renderer->BindUniformMat4f("MVP",glm::value_ptr(MVP), false);
		renderer->BindVertexInput("pos4", vbo, 4);
		//renderer->BindIBO(ibo);
		renderer->DrawLine(boneVertexData.size());
		//renderer->DrawTriangleFan(4);
	}
};

BaseApp* app = new App();