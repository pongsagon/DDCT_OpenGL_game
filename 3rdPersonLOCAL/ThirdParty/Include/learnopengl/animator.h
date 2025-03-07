// Matt edit the code to support cross fade blending of 2 clips
// 11/2/2024


#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <learnopengl/animation.h>
#include <learnopengl/bone.h>


enum MixamoBone {
	MixamoBone_Head = 5,
	MixamoBone_Hips = 0,
	MixamoBone_LeftArm = 7,
	MixamoBone_LeftFoot = 48,
	MixamoBone_LeftForeArm = 8,
	MixamoBone_LeftHand = 9,
	MixamoBone_LeftHandIndex1 = 24,
	MixamoBone_LeftHandIndex2 = 25,
	MixamoBone_LeftHandIndex3 = 26,
	MixamoBone_LeftHandMiddle1 = 27,
	MixamoBone_LeftHandMiddle2 = 28,
	MixamoBone_LeftHandMiddle3 = 29,
	MixamoBone_LeftHandPinky1 = 11,
	MixamoBone_LeftHandPinky2 = 33,
	MixamoBone_LeftHandPinky3 = 34,
	MixamoBone_LeftHandRing1 = 30,
	MixamoBone_LeftHandRing2 = 31,
	MixamoBone_LeftHandRing3 = 32,
	MixamoBone_LeftHandThumb1 = 10,
	MixamoBone_LeftHandThumb2 = 22,
	MixamoBone_LeftHandThumb3 = 23,
	MixamoBone_LeftLeg = 19,
	MixamoBone_LeftShoulder = 6,
	MixamoBone_LeftToeBase = 49,
	MixamoBone_LeftUpLeg = 18,
	MixamoBone_Neck = 4,
	MixamoBone_RightArm = 13,
	MixamoBone_RightFoot = 50,
	MixamoBone_RightForeArm = 14,
	MixamoBone_RightHand = 15,
	MixamoBone_RightHandIndex1 = 37,
	MixamoBone_RightHandIndex2 = 38,
	MixamoBone_RightHandIndex3 = 39,
	MixamoBone_RightHandMiddle1 = 40,
	MixamoBone_RightHandMiddle2 = 41,
	MixamoBone_RightHandMiddle3 = 42,
	MixamoBone_RightHandPinky1 = 17,
	MixamoBone_RightHandPinky2 = 46,
	MixamoBone_RightHandPinky3 = 47,
	MixamoBone_RightHandRing1 = 43,
	MixamoBone_RightHandRing2 = 44,
	MixamoBone_RightHandRing3 = 45,
	MixamoBone_RightHandThumb1 = 16,
	MixamoBone_RightHandThumb2 = 35,
	MixamoBone_RightHandThumb3 = 36,
	MixamoBone_RightLeg = 21,
	MixamoBone_RightShoulder = 12,
	MixamoBone_RightToeBase = 51,
	MixamoBone_RightUpLeg = 20,
	MixamoBone_Spine = 1,
	MixamoBone_Spine1 = 2,
	MixamoBone_Spine2 = 3
};




class Animator
{
public:
	Animator(Animation* animation)
	{
		m_CurrentTime = 0.0;
		m_CurrentAnimation = animation;
		m_CurrentAnimation2 = NULL;
		m_blendAmount = 0;

		m_FinalBoneMatrices.reserve(100);

		for (int i = 0; i < 100; i++)
			m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
	}

	glm::mat4 Mat4One = glm::mat4(1.0f);
	void UpdateAnimation(float dt)
	{
		m_DeltaTime = dt;
		if (m_CurrentAnimation)
		{
			m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
			m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());

			if (m_CurrentAnimation2)
			{
				m_CurrentTime2 += m_CurrentAnimation2->GetTicksPerSecond() * dt;
				m_CurrentTime2 = fmod(m_CurrentTime2, m_CurrentAnimation2->GetDuration());
			}

			CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), Mat4One);
		}
	}


	void PlayAnimation(Animation* pAnimation, Animation* pAnimation2, float time1, float time2, float blend)
	{
		m_CurrentAnimation = pAnimation;
		m_CurrentTime = time1;
		m_CurrentAnimation2 = pAnimation2;
		m_CurrentTime2 = time2;
		m_blendAmount = blend;
	}

	glm::mat4 UpdateBlend(Bone* Bone1, Bone* Bone2) {
		glm::vec3 bonePos1, bonePos2, finalPos;
		glm::vec3 boneScale1, boneScale2, finalScale;
		glm::quat boneRot1, boneRot2, finalRot;

		Bone1->InterpolatePosition(m_CurrentTime, bonePos1);
		Bone2->InterpolatePosition(m_CurrentTime2, bonePos2);
		Bone1->InterpolateRotation(m_CurrentTime, boneRot1);
		Bone2->InterpolateRotation(m_CurrentTime2, boneRot2);
		Bone1->InterpolateScaling(m_CurrentTime, boneScale1);
		Bone2->InterpolateScaling(m_CurrentTime2, boneScale2);

		finalPos = glm::mix(bonePos1, bonePos2, m_blendAmount);
		finalRot = glm::slerp(boneRot1, boneRot2, m_blendAmount);
		finalRot = glm::normalize(finalRot);
		finalScale = glm::mix(boneScale1, boneScale2, m_blendAmount);

		glm::mat4 translation = glm::translate(glm::mat4(1.0f), finalPos);
		glm::mat4 rotation = glm::toMat4(finalRot);
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), finalScale);

		return translation * rotation * scale;
	}

	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
	{
		std::string nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation;

		// Find Bone1 and Bone2 only once
		Bone* Bone1 = m_CurrentAnimation->FindBone(nodeName);
		Bone* Bone2 = (m_CurrentAnimation2) ? m_CurrentAnimation2->FindBone(nodeName) : nullptr;

		if (Bone1) {
			Bone1->Update(m_CurrentTime);
			nodeTransform = Bone1->GetLocalTransform();

			if (Bone2) {
				nodeTransform = UpdateBlend(Bone1, Bone2);
			}
		}

		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		// Get BoneInfo once, avoiding redundant map lookups
		auto it = m_CurrentAnimation->GetBoneIDMap().find(nodeName);
		if (it != m_CurrentAnimation->GetBoneIDMap().end()) {
			int index = it->second.id;
			glm::mat4 offset = it->second.offset;
			m_FinalBoneMatrices[index] = globalTransformation * offset;
		}

		// Iterate through children
		for (int i = 0; i < node->childrenCount; i++) {
			CalculateBoneTransform(&node->children[i], globalTransformation);
		}

	}

	std::vector<glm::mat4> GetFinalBoneMatrices()
	{
		return m_FinalBoneMatrices;
	}

//private:
	std::vector<glm::mat4> m_FinalBoneMatrices;
	Animation* m_CurrentAnimation;
	Animation* m_CurrentAnimation2;
	float m_CurrentTime;
	float m_CurrentTime2;  
	float m_DeltaTime;
	float m_blendAmount;

};
