#pragma once

#include "GameObject.h"
#include "../Physics.h"
#include "../ResourceManager.h"

class Gun : public GameObject
{

public:
	int FloatDir = 1;
	std::unique_ptr<CapsuleCollider> m_CapsuleCollider = nullptr;

	Gun()
	{
		s_count++;
		m_scale = glm::vec3(0.1);
		m_position = glm::vec3(5, 0, 0);

		m_CapsuleCollider = std::make_unique<CapsuleCollider>();
		m_CapsuleCollider->radius = 0.1f;
		m_CapsuleCollider->m_position = m_position;
	}

	virtual void Update(float dt) override
	{
	}


	virtual void Render(Renderer& renderer) override
	{
		Shader& shader2 = renderer.m_basicShader;
		shader2.use();

		shader2.setMat4("model", GetModelMatrix());

		Model* model = ResourceManager::Get()->GetModel("gun");

		if (model)
			model->Draw(shader2);
	}

	void Destruct() { s_count--; }

	inline static int s_count = 0;
};
