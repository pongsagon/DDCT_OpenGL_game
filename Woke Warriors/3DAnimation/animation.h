#pragma once

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include "bone.h"
#include <functional>
#include "animdata.h"
#include "model_animation.h"

struct AssimpNodeData
{
	glm::mat4 transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

struct DamageKeyframe {
	float time;
	int damage;
};


class Animation
{
public:
	Animation() = default;

	Animation(const std::string& animationPath, ModelAnim* model,float speed = 1.0f)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
		assert(scene && scene->mRootNode);
		auto animation = scene->mAnimations[0];
		m_Duration = animation->mDuration;
		m_DurationInSecond = m_Duration / m_TicksPerSecond;
		m_Speed = 1.0f;
		m_TicksPerSecond = animation->mTicksPerSecond;
		aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
		globalTransformation = globalTransformation.Inverse();
		ReadHierarchyData(m_RootNode, scene->mRootNode);
		ReadMissingBones(animation, *model);
	}

	void AddDamageKeyframe(float timeInSeconds, int damage) {
		damageKeyframes.push_back({ timeInSeconds, damage });
		cout << "Added damage keyframe at time " << timeInSeconds << " sec" << endl;
	}

	int getDamageForTime(float currentTimeInSeconds) {
		float tolerance = 0.05f; // This may need adjustment based on your specific timing precision
		for (const auto& frame : damageKeyframes) {
			if (std::abs(currentTimeInSeconds - frame.time) <= tolerance) {
				return frame.damage;
			}
		}
		return 0;
	}


	void loadAnimation(const std::string& animationPath, ModelAnim* model,float speed = 1.0f)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
		assert(scene && scene->mRootNode);
		auto animation = scene->mAnimations[0];  // Assuming first animation is what we need

		m_Duration = animation->mDuration;
		m_Speed = speed;
		m_TicksPerSecond = animation->mTicksPerSecond != 0 ? animation->mTicksPerSecond : 25.0f;  // Default to 25 if not specified

		std::cout << "Loaded Animation: " << animationPath << " with duration: " << m_Duration << " and ticks per second: " << m_TicksPerSecond << std::endl;

		aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
		globalTransformation = globalTransformation.Inverse();
		ReadHierarchyData(m_RootNode, scene->mRootNode);
		ReadMissingBones(animation, *model);
	}

	~Animation()
	{
	}

	Bone* FindBone(const std::string& name)
	{
		auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
			[&](const Bone& Bone)
			{
				return Bone.GetBoneName() == name;
			}
		);
		if (iter == m_Bones.end()) return nullptr;
		else return &(*iter);
	}

	void setAnimationSpeed(float speed)
	{
		m_Speed = speed;
	}


	
	inline float GetTicksPerSecond() { return m_TicksPerSecond; }
	inline float GetDuration() { return m_Duration;}
	inline float GetSpeed() { return m_Speed; }
	inline const AssimpNodeData& GetRootNode() { return m_RootNode; }
	inline const std::map<std::string,BoneInfo>& GetBoneIDMap() 
	{ 
		return m_BoneInfoMap;
	}

private:
	void ReadMissingBones(const aiAnimation* animation, ModelAnim& model)
	{
		int size = animation->mNumChannels;

		auto& boneInfoMap = model.GetBoneInfoMap();//getting m_BoneInfoMap from Model class
		int& boneCount = model.GetBoneCount(); //getting the m_BoneCounter from Model class

		//reading channels(bones engaged in an animation and their keyframes)
		for (int i = 0; i < size; i++)
		{
			auto channel = animation->mChannels[i];
			std::string boneName = channel->mNodeName.data;

			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				boneInfoMap[boneName].id = boneCount;
				boneCount++;
			}
			m_Bones.push_back(Bone(channel->mNodeName.data,
				boneInfoMap[channel->mNodeName.data].id, channel));
		}

		m_BoneInfoMap = boneInfoMap;
	}

	void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src)
	{
		assert(src);

		dest.name = src->mName.data;
		dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
		dest.childrenCount = src->mNumChildren;

		for (int i = 0; i < src->mNumChildren; i++)
		{
			AssimpNodeData newData;
			ReadHierarchyData(newData, src->mChildren[i]);
			dest.children.push_back(newData);
		}
	}
	float m_Duration;
	float m_Speed;
	int m_TicksPerSecond;
	float currentDuration;
	std::vector<Bone> m_Bones;
	AssimpNodeData m_RootNode;
	float m_DurationInSecond;
	std::map<std::string, BoneInfo> m_BoneInfoMap;
	std::vector<DamageKeyframe> damageKeyframes;
};

