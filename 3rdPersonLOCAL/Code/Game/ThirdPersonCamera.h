#pragma once

#include "../Camera.h"

class ThirdPersonCamera : public Camera
{
public:
	ThirdPersonCamera(GameObject& gameObject)
		: m_target(gameObject)
	{
	}

	double TURN_Y = 0.0f;
	double TURN_X = 0.0f;


	// 0 - 719 % 720
	float yRotation = 0.0f;

	virtual void Update(float dt) override
	{
		//glm::vec3 m { glm::cos(TURN_Y), 0, glm::sin(TURN_Y) };
		pair mouseChange = Input::getMousePosChange();
		

		yRotation += glm::radians(30.0f) * -mouseChange.first * dt;

		float rightAngle = yRotation - (glm::pi<float>() / 2.0f);

		glm::vec3 forward{ glm::sin(yRotation), 0, glm::cos(yRotation) };
		glm::vec3 right{ glm::sin(rightAngle), 0, glm::cos(rightAngle) };


		float backOffset = 2.0f;
		float sideOffset = 0.5f;

		if (Input::GetMouseButton(GLFW_MOUSE_BUTTON_2))
		{
			backOffset = 1.0f;
		}


		const glm::vec3 offset = glm::vec3(0, 2, -3);
		const glm::vec3 targetPosition = m_target.m_position + (right * sideOffset);
		m_position = targetPosition - (backOffset * forward);
		m_position.y += 1.25f;
		m_lookAt = targetPosition + (backOffset * forward);
		m_lookAt.y += 1.5f;

	}

protected:
	GameObject& m_target;
};