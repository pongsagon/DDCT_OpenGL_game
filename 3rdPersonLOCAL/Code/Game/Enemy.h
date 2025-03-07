#pragma once

#include "GameObject.h"
#include <learnopengl/model_animation.h>
#include <learnopengl/animator.h>

class Enemy : public GameObject
{
public:
	Enemy()
		: m_model("Assets/Models/mixamo/doozy/doozy.dae")
		, idleAnimation("Assets/Models/mixamo/doozy/Fight Idle.dae", &m_model)
		, walkAnimation("Assets/Models/mixamo/doozy/Fight Idle.dae", &m_model)
		, runAnimation("Assets/Models/mixamo/doozy/Run.dae", &m_model)
		, punchAnimation("Assets/Models/mixamo/doozy/Fight Idle.dae", &m_model)
		, knockAnimation("Assets/Models/mixamo/doozy/Slipping.dae", &m_model)
	{
		m_animator = std::make_unique<Animator>(&idleAnimation);
		m_scale = glm::vec3(1.0f);
		m_position = glm::vec3(5, 0, 5);
	}

	virtual void Update(float dt) override
	{
		// DO AI STUFF


		m_animator->UpdateAnimation(dt);
	}

	virtual void Render(Renderer& renderer) override
	{
		renderer.DrawAnimation(m_model, m_position, m_scale, m_rotation, m_animator->GetFinalBoneMatrices());
	}

	Model m_model;
	std::unique_ptr<Animator> m_animator;

	Animation idleAnimation;
	Animation walkAnimation;
	Animation runAnimation;
	Animation punchAnimation;
	Animation knockAnimation;
};