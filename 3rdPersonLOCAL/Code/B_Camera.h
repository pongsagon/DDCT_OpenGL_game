#pragma once

#include "BanKEngine.h"
#include "Input.h"

class B_Camera : public BanKBehavior
{
public:

	float m_fov = 60.0f;
	glm::vec3 m_lookAt = glm::vec3(0.0f);



	virtual glm::mat4 GetViewMatrix() const
	{
		return glm::lookAt(GameObject->Transform.wPosition, m_lookAt, glm::vec3(0, 1, 0));
		//return glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0.0), glm::vec3(0, 1, 0));
	}
	virtual glm::mat4 GetProjectionMatrix() const
	{ 
		/*glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)1200 / (float)800, 0.1f, 100.0f);
		return projection;*/

		glm::vec2 windowSize = Application::Get().GetWindowSize();
		return glm::perspective(glm::radians(m_fov), (float)windowSize.x / (float)windowSize.y, 0.1f, 99999.0f);
	}
	glm::mat4 GetViewProjectionMatrix() const
	{
		return GetProjectionMatrix() * GetViewMatrix();
	}



	void Update() {

		//float Speed = 10 * Time.Deltatime;
		//if (Input::GetKey(GLFW_KEY_W)) {
		//	GameObject->Transform.wPosition += GameObject->Transform.getForwardVector()* Speed;
		//}
		//else if (Input::GetKey(GLFW_KEY_S))
		//{
		//	GameObject->Transform.wPosition -= GameObject->Transform.getForwardVector()* Speed;
		//}

		//if (Input::GetKey(GLFW_KEY_A)) {
		//	GameObject->Transform.wPosition += GameObject->Transform.getLeftVector() * Speed;
		//}
		//else if (Input::GetKey(GLFW_KEY_D))
		//{
		//	GameObject->Transform.wPosition -= GameObject->Transform.getLeftVector() * Speed;
		//}

		//if (Input::GetKey(GLFW_KEY_E)) {
		//	GameObject->Transform.wPosition.y += Speed;
		//}
		//else if (Input::GetKey(GLFW_KEY_Q))
		//{
		//	GameObject->Transform.wPosition.y -= Speed;
		//}

	}


};