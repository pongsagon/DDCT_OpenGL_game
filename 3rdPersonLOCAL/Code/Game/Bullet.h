#pragma once

#include "GameObject.h"

class Bullet : public GameObject
{
public:
	Bullet(const glm::vec3& position = glm::vec3{ 0.0f }, const glm::vec3& forward = glm::vec3{ 0.0f, 0.0f, 1.0f })
		: m_forward(forward)
	{
		m_position = position;
		m_scale = glm::vec3(0.2f);
	}

	virtual void Update(float dt) override
	{
		m_lifeTimer += dt;

		if (m_lifeTimer < s_lifeDuration)
		{
			m_position += dt * s_bulletSpeed * m_forward;
		}
	}

	virtual void Render(Renderer& renderer) override
	{
		Model* model = ResourceManager::Get()->GetModel("bullet");

		if (model)
		{
			Shader& shader = renderer.m_basicShader;
			shader.use();
			shader.setMat4("model", GetModelMatrix());
			model->Draw(shader);
		}
	}

	float m_lifeTimer = 0.0f;
	glm::vec3 m_forward;

	inline static float s_lifeDuration = 2.0f;
	inline static float s_bulletSpeed = 3.0f;
};