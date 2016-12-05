// TestNewProject.cpp : Defines the entry point for the console application.
//
#include "../Base/base.h"
#include "../Base/BaseApp.h"
#include "../OpenGLRenderer/OpenGLRenderer.h"
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <vector>
#include <map>
#include <unordered_map>
#include <iostream>
#include "..\OpenGLRenderer/include/glm/gtx/vector_angle.hpp"

BaseRenderer* renderer = new Renderer();


class Quaternion
{
public:
	Quaternion(): s(1.f), i(0), j(0), k(0) {}
	Quaternion(float s, float i, float j, float k): s(s), i(i), j(j), k(k) {}
	Quaternion(const aiQuaternion& aiquat):s(aiquat.w), i(aiquat.x), j(aiquat.y), k(aiquat.z)	{}
	Quaternion(const glm::vec4& v) : s(v.w), i(v.x), j(v.y), k(v.z) {}
	Quaternion(float s, const glm::vec3& v) : s(s), i(v.x), j(v.y), k(v.z) {}
	Quaternion(const glm::vec3& axis, float angle)
	{
		s = cos(angle / 2);
		glm::vec3 v = sin(angle / 2) * axis;
		i = v.x;
		j = v.y;
		k = v.z;

		float invmag = 1.f/magnitude();
		*this = *this * invmag;
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
	float angle()
	{
		return 2 * acos(s);
	}
	glm::vec3 axis()
	{
		float invsqrt = 1 / sqrt(1 - s*s);
		return glm::vec3(i,j,k) * invsqrt;
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


struct SkeletonNode
{
	aiNode* node;
	aiNodeAnim* anim;
	std::vector<SkeletonNode*> children;
	SkeletonNode* parent;
	aiMatrix4x4 initTransformation;
	Quaternion initQuaternion;
	Quaternion deltaQuaternion;
};
class App: public BaseApp
{
private:
	float vertexData[8] = {-0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f};
	std::vector<float> boneVertexData;
	std::vector<float> pathVertexData;
	std::vector<float> floorVertexData;
	std::vector<float> targetVertexData;
	std::vector<glm::vec3> pathKnots;
	std::vector<glm::vec3> intermediatePathKnots;
	std::vector<glm::vec3> targetVertex;
	std::map<float, float> arcLengthMap;
	int indexData[4] = { 0,1,2,3 };
	char* vertexShaderSource[1];
	char* pixelShaderSource[1];
	int bonevbo = 0;
	int pathvbo = 0;
	int floorvbo = 0;
	int targetvbo = 0;
	int ibo = 0;
	int vertexShader = 0;
	int pixelShader = 0;
	int shaderProgram = 0;
	glm::mat4 MVP;
	glm::vec3 lookAtDir;
	glm::vec3 cameraPos;
	glm::vec3 upVec;
	glm::vec3 leftVec;
	glm::vec3 targetPos;
	SkeletonNode* skeleton;
	float current_tick = 0.0f;
	int max_tick = 0;
	float current_dist = 0.0f;
	float max_dist = 0.0f;
	glm::mat4 pos_matrix = glm::mat4(1.0f);
	SkeletonNode* end_effector = 0;
	SkeletonNode* skeleton_local_root = 0;
	std::unordered_map<SkeletonNode*, float> constraint;

	int projectIndex = 3;

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
		skeleton->children[0]->parent = NULL;
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
				newNode->parent = parent;
				newNode->initQuaternion = aiQuaternion(aiMatrix3x3(child->mTransformation));
				newNode->initTransformation = child->mTransformation;
				for (int i = 0; i < animation->mNumChannels; ++i)
				{
					if (animation->mChannels[i]->mNodeName == child->mName)
					{
						newNode->anim = animation->mChannels[i];
						break;
					}
				}
				if (strcmp(newNode->node->mName.C_Str() ,"Bip01 L Finger12") == 0)
				{
					end_effector = newNode;
				}
				else if (strcmp(newNode->node->mName.C_Str(), "Bip01 Spine1") == 0)
				{
					skeleton_local_root = newNode;
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
		bonevbo = renderer->CreateVBO(vertexData, 8);
		pathvbo = renderer->CreateVBO(vertexData, 8);
		floorvbo = renderer->CreateVBO(vertexData, 8);
		targetvbo = renderer->CreateVBO(vertexData, 8);
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
		cameraPos = glm::vec3(0.0, 5.0, 12.0);
		lookAtDir = glm::vec3(0.0, -0.4, -1.0);
		upVec = glm::vec3(0.0, 1.0, 0.0);
		leftVec = glm::normalize(glm::cross(lookAtDir, upVec));
		
		MVP = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 100.0f, -100.0f) * glm::lookAt(cameraPos, lookAtDir + cameraPos, upVec);

		if (projectIndex == 2)
		{
			// Path predefined
			pathKnots.push_back(glm::vec3(-5.0, 0.0, 5.0));
			pathKnots.push_back(glm::vec3(2.0, 0.0, 5.0));
			pathKnots.push_back(glm::vec3(1.0, 0.0, -1.0));
			pathKnots.push_back(glm::vec3(-2.0, 0.0, -2.0));
			pathKnots.push_back(glm::vec3(-5.0, 0.0, -1.0));
			pathKnots.push_back(glm::vec3(0.0, 0.0, 1.0));
			pathKnots.push_back(glm::vec3(4.0, 0.0, -1.0));
			pathKnots.push_back(glm::vec3(5.0, 0.0, -5.0));


			// generate path data
			pathVertexData.clear();
			bezierInterpolation(pathKnots);
			renderer->FillVBO(pathvbo, &pathVertexData[0], pathVertexData.size());
		}

		// floor data
		std::vector<glm::vec3> floorVertex;
		floorVertex.push_back(glm::vec3(-5.f, 0.f, 5.f));
		floorVertex.push_back(glm::vec3(-5.f, 0.f, -5.f));
		floorVertex.push_back(glm::vec3(-5.f, 0.f, -5.f));
		floorVertex.push_back(glm::vec3(5.f, 0.f, -5.f));
		floorVertex.push_back(glm::vec3(5.f, 0.f, -5.f));
		floorVertex.push_back(glm::vec3(5.f, 0.f, 5.f));
		floorVertex.push_back(glm::vec3(5.f, 0.f, 5.f));
		floorVertex.push_back(glm::vec3(-5.f, 0.f, 5.f));

		for (int i = 0; i < floorVertex.size(); ++i)
		{
			floorVertexData.push_back(floorVertex[i].x);
			floorVertexData.push_back(floorVertex[i].y);
			floorVertexData.push_back(floorVertex[i].z);
			floorVertexData.push_back(1.f);
		}
		renderer->FillVBO(floorvbo, &floorVertexData[0], floorVertexData.size());

		// target cube data for proj 3
		targetVertex.push_back(glm::vec3(-0.25, 0.25, 0));
		targetVertex.push_back(glm::vec3(-0.25, -0.25, 0));
		targetVertex.push_back(glm::vec3(-0.25, -0.25, 0));
		targetVertex.push_back(glm::vec3(0.25, -0.25, 0));
		targetVertex.push_back(glm::vec3(0.25, -0.25, 0));
		targetVertex.push_back(glm::vec3(0.25, 0.25, 0));
		targetVertex.push_back(glm::vec3(0.25, 0.25, 0));
		targetVertex.push_back(glm::vec3(-0.25, 0.25, 0));

		targetPos = glm::vec3(0.0f, 5.0f, 0.0f);
	}

	
	/* Read skeleton and interpolate the animation at runtime */
	void readSkeletonHelper(SkeletonNode* parent, glm::vec4 parent_pos, glm::mat4 parent_matrix, SkeletonNode* current, float current_tick)
	{	
		glm::mat4 matrix;
		/* if not animation provided, use the default transformation info */
		if (projectIndex == 3)
			current->anim = NULL;
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
		glm::mat4 matrix(m[0][0], m[1][0], m[2][0], m[3][0], m[0][1], m[1][1], m[2][1], m[3][1], m[0][2], m[1][2], m[2][2], m[3][2], m[0][3], m[1][3], m[2][3], m[3][3]);
		return (matrix);
	}
	aiMatrix4x4 matrixConverter(const glm::mat4& m)
	{
		aiMatrix4x4 aiMatrix(m[0][0], m[1][0], m[2][0], m[3][0], m[0][1], m[1][1], m[2][1], m[3][1], m[0][2], m[1][2], m[2][2], m[3][2], m[0][3], m[1][3], m[2][3], m[3][3]);
		return aiMatrix;
	}
	glm::vec3 vectorConverter(const aiVector3t<float>& m)
	{
		return glm::vec3(m.x, m.y, m.z);
	}

	void bezierInterpolation(const std::vector<glm::vec3>& v)
	{
		assert(v.size() > 1);
		float scale = 0.1;
		for (int i = 0; i < v.size(); ++i)
		{
			if (i == 0)
			{
				glm::vec3 p1 = v[i];
				glm::vec3 p2 = v[i + 1];
				glm::vec3 tangent = v[i + 1] - v[i];
				glm::vec3 q1 = p1 + scale*tangent;

				intermediatePathKnots.push_back(p1);
				intermediatePathKnots.push_back(q1);
			}
			else if (i == v.size() - 1)
			{
				glm::vec3 p0 = v[i - 1];
				glm::vec3 p1 = v[i];
				glm::vec3 tangent = p1 - p0;
				glm::vec3 q0 = p1 - scale*tangent;
				
				intermediatePathKnots.push_back(q0);
				intermediatePathKnots.push_back(p1);
			}
			else
			{
				glm::vec3 p0 = v[i - 1];
				glm::vec3 p1 = v[i];
				glm::vec3 p2 = v[i + 1];
				glm::vec3 tangent = glm::normalize(p2 - p0);
				glm::vec3 q0 = p1 - scale*tangent*glm::length(p1 - p0);
				glm::vec3 q1 = p1 + scale*tangent*glm::length(p2 - p1);

				intermediatePathKnots.push_back(q0);
				intermediatePathKnots.push_back(p1);
				intermediatePathKnots.push_back(q1);

			}
		}

		// fill path curve data
		for (int cur_tick = 0; cur_tick < 5000; ++cur_tick)
		{
			glm::vec3 cur_point = deCasteljau(intermediatePathKnots, cur_tick / 5000.f);
			if (cur_tick > 0 && cur_tick < 5000 - 1)
			{
				pathVertexData.push_back(cur_point[0]);
				pathVertexData.push_back(cur_point[1]);
				pathVertexData.push_back(cur_point[2]);
				pathVertexData.push_back(1.0f);
			}
				pathVertexData.push_back(cur_point[0]);
				pathVertexData.push_back(cur_point[1]);
				pathVertexData.push_back(cur_point[2]);
				pathVertexData.push_back(1.0f);
		}


		// generate arc length table
		arcLengthMap.clear();
		arcLengthMap[0.0f] = 0.0f;
		std::vector<float> segment_list;
		float threshold = 0.00001f;
		float max_interval = 0.001f;
		segment_list.push_back(0.0f);
		segment_list.push_back(1.0f);
		std::unordered_map<float, float> invArcLengthMap;
		while (segment_list.size() > 0)
		{
				float ua = segment_list[0];
				float ub = segment_list[1];
				float um = 0.5*(ua + ub);
				float A = glm::length(deCasteljau(intermediatePathKnots, ua) - deCasteljau(intermediatePathKnots, um));
				float B = glm::length(deCasteljau(intermediatePathKnots, ub) - deCasteljau(intermediatePathKnots, um));
				float C = glm::length(deCasteljau(intermediatePathKnots, ub) - deCasteljau(intermediatePathKnots, ua));
				float d = A + B - C;

				if (d > threshold && abs(ua - ub) > max_interval)
				{
					segment_list.insert(segment_list.begin() + 1, um);
					segment_list.insert(segment_list.begin() + 1, um);
				}
				else
				{
					invArcLengthMap[um] = invArcLengthMap[ua] + A;
					invArcLengthMap[ub] = invArcLengthMap[um] + B;
					segment_list.erase(segment_list.begin());
					segment_list.erase(segment_list.begin());
				}
		}
		auto end_iter = invArcLengthMap.end();
		end_iter--;
		max_dist = end_iter->second;

		for (auto item : invArcLengthMap)
		{
			arcLengthMap[item.second] = item.first;
		}
		
	}

	glm::vec3 deCasteljau(std::vector<glm::vec3> controlPoints, float t)
	{
		for (int i = 0; i < controlPoints.size()-1; ++i)
		{
			for (int j = 0; j < controlPoints.size()-i-1; ++j)
			{
				controlPoints[j] = controlPoints[j] * (1 - t) + controlPoints[j + 1] * t;
			}
		}
		return controlPoints[0];
	}

	glm::vec3 pathFindByLength(float length)
	{
		auto it = arcLengthMap.lower_bound(length);
		if (it == arcLengthMap.end())
			it--;
		return deCasteljau(intermediatePathKnots, it->second);
	}

	float walkingSpeedFunc(float maxWalkingSpeed)
	{
		float accel_dist = 0.1*max_dist;
		float min_value_fix = 0.05;
		if (current_dist < accel_dist)
		{
			float speed = maxWalkingSpeed * sinf(current_dist / accel_dist * M_PI*0.5);
			if (speed < min_value_fix)
				speed = min_value_fix;
			return speed;
		}
		if (current_dist > max_dist - accel_dist)
		{
			float speed = maxWalkingSpeed * cosf((current_dist - max_dist + accel_dist) / accel_dist * M_PI*0.5);
			if (speed < min_value_fix)
				speed = min_value_fix;
			return speed;
		}
		return maxWalkingSpeed;
	}

	glm::vec3 getGlobalPos(SkeletonNode* node)
	{
		glm::vec4 e_pos(0.f, 0.f, 0.f, 1.f);
		for (SkeletonNode* e = node; e != NULL; e = e->parent)
		{
			e_pos = matrixConverter(e->node->mTransformation) * e_pos;
		}
		e_pos = pos_matrix * e_pos;
		return glm::vec3(e_pos);
	}

	bool CCD()
	{
		float epsilon = 0.2f;
		glm::vec3 initPos = getGlobalPos(end_effector);
		glm::vec3 nextPos = initPos;
		if (glm::distance(initPos, targetPos) < 0.01)
		{
			return true;
		}
		do {
			initPos = nextPos;
			for (SkeletonNode* current_node = end_effector; current_node->parent != skeleton_local_root; current_node = current_node->parent)
			{
				glm::vec3 jk = getGlobalPos(current_node->parent);
				assert(jk == jk);
				glm::vec3 vck = glm::normalize(nextPos - jk);
				glm::vec3 vdk = glm::normalize(targetPos - jk);
				float angle = glm::acos(glm::clamp(-1.0f,1.0f, glm::dot(vck, vdk)));
				
				glm::vec3 vk = glm::normalize(glm::cross(vck, vdk));
				Quaternion out(vk, angle);

				Quaternion temp = out * current_node->deltaQuaternion;

				// Constraint
				/*if (temp.angle() > M_PI / 3)
					temp = Quaternion(cos(M_PI / 6), temp.axis() * sinf(M_PI/6));*/
				current_node->deltaQuaternion = temp;

				current_node->node->mTransformation = matrixConverter(current_node->deltaQuaternion.matrix()) * current_node->initTransformation;

				nextPos = getGlobalPos(end_effector);
				if (glm::distance(nextPos, targetPos) < 0.01)
				{
					return true;
				}
			}
		} while (glm::distance(initPos, nextPos) > 0.1);
		 
		return false;
	}
	/* Update animation per frame */
	void Update()
	{
		BaseApp::Update();
		{
			float cameraSpeed = 0.4;
			float targetSpeed = 0.4;
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
			if (InputManager::KeyDown(SDL_SCANCODE_UP))
			{
				targetPos += targetSpeed * glm::vec3(0, 0, -1);
			}
			if (InputManager::KeyDown(SDL_SCANCODE_DOWN))
			{
				targetPos += targetSpeed * glm::vec3(0, 0, 1);
			}
			if (InputManager::KeyDown(SDL_SCANCODE_LEFT))
			{
				targetPos += targetSpeed * glm::vec3(-1, 0, 0);
			}
			if (InputManager::KeyDown(SDL_SCANCODE_RIGHT))
			{
				targetPos += targetSpeed * glm::vec3(1, 0, 0);
			}
		}


		if (projectIndex == 3)
		{
			if (!CCD())
			{

				// Draw path

				glm::vec3 direction = targetPos - glm::vec3(pos_matrix[3][0], 0.0f, pos_matrix[3][2]);
				direction = glm::normalize(direction);
				pathVertexData.clear();
				pathVertexData.push_back(targetPos.x);
				pathVertexData.push_back(0.0f);
				pathVertexData.push_back(targetPos.z);
				pathVertexData.push_back(1.0f);
				pathVertexData.push_back(pos_matrix[3][0]);
				pathVertexData.push_back(0.0f);
				pathVertexData.push_back(pos_matrix[3][1]);
				pathVertexData.push_back(1.0f);

				pos_matrix[3][0] += (direction * 0.01f)[0];
				pos_matrix[3][2] += (direction * 0.01f)[2];
			}
		}
		
		
		if (pathKnots.size() > 0)
		{
			float maxWalkingSpeed = 1.0f;	// unit per second
			float walkingSpeed = walkingSpeedFunc(maxWalkingSpeed);
			current_dist += walkingSpeed / 60.0f;
			if (current_dist >= max_dist)
				current_dist = 0;

			float cycle_per_unit = 0.3f;
			float animationSpeed = cycle_per_unit * walkingSpeed;	// cycle per second
			current_tick += max_tick * animationSpeed / 60;
			if (current_tick >= max_tick)
				current_tick = 0;

			glm::vec3 current_pos = pathFindByLength(current_dist);
			glm::vec3 looking_pos = pathFindByLength(current_dist + 0.8f);
			pos_matrix = glm::rotate(pos_matrix, glm::acos(glm::dot(glm::normalize(looking_pos - current_pos), glm::vec3(1., 0., 0.))) + (float)M_PI, glm::vec3(0.f, 1.f, 0.f));
			pos_matrix[3][0] = current_pos[0];
			pos_matrix[3][1] = current_pos[1];
			pos_matrix[3][2] = current_pos[2];
		}		



		leftVec = glm::normalize(glm::cross(lookAtDir, upVec));
		MVP = glm::perspectiveFov((float)M_PI/2, 100.f,100.f, 0.1f, 100.0f) * glm::lookAt(cameraPos, lookAtDir + cameraPos, upVec);

		// Update bones
		boneVertexData.clear();
		readSkeletonHelper(NULL, glm::vec4(0.0,0.0,0.0,1.0), pos_matrix, skeleton, current_tick);
		renderer->FillVBO(bonevbo, &boneVertexData[0], boneVertexData.size());

		// Fill path data
		if (pathVertexData.size() > 0)
			renderer->FillVBO(pathvbo, &pathVertexData[0], pathVertexData.size());

		renderer->FillVBO(floorvbo, &floorVertexData[0], floorVertexData.size());
		 
		// Fill target data
		targetVertexData.clear();
		for (int i = 0; i < targetVertex.size(); ++i)
		{
			glm::vec3 dest = targetPos + targetVertex[i];
			targetVertexData.push_back(dest.x);
			targetVertexData.push_back(dest.y);
			targetVertexData.push_back(dest.z);
			targetVertexData.push_back(1.0f);
		}
		renderer->FillVBO(targetvbo, &targetVertexData[0], targetVertexData.size());
	}

	/* Render content per frame */
	void Render()
	{
		BaseApp::Render();
		renderer->SetClearColor(1.0f, 0.0, 0.0, 1.0);
		renderer->ClearScreen();

		renderer->UseShaderProgram(shaderProgram);
		renderer->BindUniformMat4f("MVP",glm::value_ptr(MVP), false);

		// Draw Bones
		renderer->BindVertexInput("pos4", bonevbo, 4);
		renderer->DrawLine(boneVertexData.size());

		// Draw Path
		if (pathVertexData.size() > 0)
		{
			renderer->BindVertexInput("pos4", pathvbo, 4);
			renderer->DrawLine(pathVertexData.size());
		}

		renderer->BindVertexInput("pos4", floorvbo, 4);
		renderer->DrawLine(floorVertexData.size());

		renderer->BindVertexInput("pos4", targetvbo, 4);
		renderer->DrawLine(targetVertexData.size());
	}
};

BaseApp* app = new App();