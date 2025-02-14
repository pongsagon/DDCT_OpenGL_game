#pragma once

#include "Camera.h"
#include "Input.h"

class EditorCamera : public Camera
{
public:
    EditorCamera()
    {
        glm::vec2 windowSize = Application::Get().GetWindowSize();
        lastX = windowSize.x / 2.0;
        lastY = windowSize.y / 2.0;
    }

	virtual void Update(float dt) override
	{
        float velocity = m_movementSpeed * dt;

        if (Input::GetKey(GLFW_KEY_W))
            m_position += m_front * velocity;

        if (Input::GetKey(GLFW_KEY_S))
            m_position -= m_front * velocity;

        if (Input::GetKey(GLFW_KEY_A))
            m_position -= m_right * velocity;

        if (Input::GetKey(GLFW_KEY_D))
            m_position += m_right * velocity;

        if (Input::IsMouseMoved() && Input::GetMouseButton(GLFW_MOUSE_BUTTON_2))
        {
            glm::vec2 cursorPos = Application::Get().GetCursorPosition();

            if (firstMouse)
            {
                lastX = cursorPos.x;
                lastY = cursorPos.y;
                firstMouse = false;
            }

            float xoffset = cursorPos.x - lastX;
            float yoffset = lastY - cursorPos.y; // reversed since y-coordinates go from bottom to top

            lastX = cursorPos.x;
            lastY = cursorPos.y;

            this->ProcessMouseMovement(xoffset, yoffset);
        }
	}


    virtual glm::mat4 GetViewMatrix() const override
    {
        return glm::lookAt(m_position, m_position + m_front, m_up);
    }

private:
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
    {
        xoffset *= m_mouseSensitivity;
        yoffset *= m_mouseSensitivity;

        m_yaw += xoffset;
        m_pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (m_pitch > 89.0f)
                m_pitch = 89.0f;
            if (m_pitch < -89.0f)
                m_pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        UpdateCameraVectors();
    }

    void UpdateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        front.y = sin(glm::radians(m_pitch));
        front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        m_right = glm::normalize(glm::cross(m_front, m_worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        m_up = glm::normalize(glm::cross(m_right, m_front));
    }

    float m_yaw = -90.0f;
    float m_pitch = 0.0f;

    float lastX;
    float lastY;
    bool firstMouse = true;


    float m_mouseSensitivity = 0.1f;
    float m_movementSpeed = 2.5f;
    glm::vec3 m_up { 0.0f, 1.0f, 0.0f };
    glm::vec3 m_worldUp { 0.0f, 1.0f, 0.0f };
    glm::vec3 m_front { 0.0f, 0.0f, -1.0f };
    glm::vec3 m_right { 1.0f, 0.0f, 0.0f };
};