#pragma once

#include "../Renderer.h"

class GameObject
{
public:
	virtual ~GameObject() = default;
	virtual void Update(float dt) {}
	virtual void Render(Renderer& renderer) {}

	glm::mat4 GetModelMatrix() const
	{
		return glm::translate(glm::mat4(1.0f), m_position)
			* glm::mat4(glm::quat(m_rotation))
			* glm::scale(glm::mat4(1.0f), m_scale);
	}

	glm::vec3 m_position{ 0.0f };
	glm::vec3 m_rotation{ 0.0f };
	glm::vec3 m_scale{ 1.0f };
};