#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Application.h"

class Camera
{
public:
	Camera()
		: m_position {0.0f}
		, m_lookAt {0.0f}
	{

	}

	virtual ~Camera() = default;

	virtual void Update(float dt) {}

	virtual glm::mat4 GetViewMatrix() const
	{
		return glm::lookAt(m_position, m_lookAt, glm::vec3(0, 1, 0));
	}

	virtual glm::mat4 GetProjectionMatrix() const
	{
		glm::vec2 windowSize = Application::Get().GetWindowSize();
		return glm::perspective(glm::radians(m_fov), (float)windowSize.x / (float)windowSize.y, 0.1f, 100.0f);
	}

	glm::mat4 GetViewProjectionMatrix() const
	{
		return GetProjectionMatrix() * GetViewMatrix();
	}

	glm::vec3 GetPosition() const
	{
		return m_position;
	}

	void SetPosition(const glm::vec3& position)
	{
		m_position = position;
	}

	glm::vec3 GetLookAt() const
	{
		return m_lookAt;
	}

	void SetLookAt(const glm::vec3& lookAt)
	{
		m_lookAt = lookAt;
	}

protected:
	float m_fov = 60.0f;
	glm::vec3 m_position;
	glm::vec3 m_lookAt;
};