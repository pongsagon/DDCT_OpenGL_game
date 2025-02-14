#pragma once

#include <GLFW/glfw3.h>
#include "Camera.h"
#include "Application.h"
#include "Input.h"
#include "_Car.h"

class FollowCamera : public Camera
{
public:
    bool isFollowView = true;

    FollowCamera(Car& car)
        : m_targetCarPos(car)
    {
        m_position = glm::vec3(0, 0, 0);
        m_targetPosition = m_position;
    }

    void Update(float dt)
    {
        if (Input::GetKeyDown(GLFW_KEY_SPACE))
            isFollowView = !isFollowView;

        glm::vec3 carDirection = m_targetCarPos.m_forward;
        glm::vec3 k = -glm::normalize(m_targetCarPos.m_forward);

        if (!isFollowView)
            k = glm::normalize(glm::cross({ 0, 1, 0 }, carDirection));

        m_targetPosition = m_targetCarPos.m_position + (k * m_cameraDistance);
        m_targetPosition.y += m_cameraY;

        glm::vec3 moveDirection = m_targetPosition - m_position;
        m_position = Lerp(m_position, m_targetPosition, dt * m_cameraSpeed);
    }

    virtual glm::mat4 GetViewMatrix() const override
    {
        return glm::lookAt(m_position, m_targetCarPos.m_position, glm::vec3(0, 1, 0));
    }

    virtual glm::mat4 GetProjectionMatrix() const override
    {
        glm::vec2 windowSize = Application::Get().GetWindowSize();
        return glm::perspective(glm::radians(m_zoom), (float)windowSize.x / (float)windowSize.y, 0.1f, 1000.0f);
    }

    float GetZoom() const
    {
        return m_zoom;
    }

private:
    float m_cameraSpeed = 15.0f;
    float m_delayTimer = 0.0f;
    float m_cameraY = 2.0f;
    float m_cameraDistance = 4.25f;

    float theta = 30.0f;
    Car& m_targetCarPos;
    glm::vec3 m_targetPosition;
    float m_zoom = 60.0f;
};









class FollowCamera_B : public Camera
{
public:
	bool isFollowView = true;

    FollowCamera_B(B_Car& car)
        : MyCar(car)
	{
	}

	void Update(float dt) 
	{
        m_position = MyCar.CameraHolder->Transform.getWorldPosition();
	}

	virtual glm::mat4 GetViewMatrix() const override
	{
		//return glm::lookAt(m_position, MyCar.CameraLookat->Transform.getWorldPosition(), glm::vec3(0, 1, 0));

        glm::vec3 cameraPosition = m_position;
        glm::vec3 cameraOrientation = MyCar.CameraHolder->Transform.getWorldRotation();
        return glm::lookAt(cameraPosition, MyCar.CameraLookat->Transform.getWorldPosition(), MyCar.CameraHolder->Transform.getUpVector());
	}

	virtual glm::mat4 GetProjectionMatrix() const override
	{
		glm::vec2 windowSize = Application::Get().GetWindowSize();
		return glm::perspective(glm::radians(m_zoom), (float)windowSize.x / (float)windowSize.y, 0.1f, 12345.0f);
	}

	float GetZoom() const
	{
		return m_zoom;
	}

private:
	float m_cameraSpeed = 15.0f;
	float m_delayTimer = 0.0f;
	float m_cameraY = 2.0f;
	float m_cameraDistance = 4.25f;

	float theta = 30.0f;
    B_Car& MyCar;
	float m_zoom = 60.0f;
};









//
//
//enum Camera_Movement {
//	FORWARD,
//	BACKWARD,
//	LEFT,
//	RIGHT,
//	UP,
//	DOWN
//};
//
//const float YAW = -90.0f;
//const float PITCH = 0.0f;
//const float SPEED = 2.5f;
//const float SENSITIVITY = 0.1f;
//const float ZOOM = 45.0f;
//
//class B_Camera : public Camera
//{
//public:
//	glm::vec3 Position;
//	glm::vec3 Front;
//	glm::vec3 Up;
//	glm::vec3 Right;
//	glm::vec3 WorldUp;
//	float Yaw;
//	float Pitch;
//	float MovementSpeed;
//	float MouseSensitivity;
//	float Zoom;
//
//	B_Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
//		: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
//	{
//		Position = position;
//		WorldUp = up;
//		Yaw = yaw;
//		Pitch = pitch;
//		updateCameraVectors();
//	}
//
//	B_Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
//		: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
//	{
//		Position = glm::vec3(posX, posY, posZ);
//		WorldUp = glm::vec3(upX, upY, upZ);
//		Yaw = yaw;
//		Pitch = pitch;
//		updateCameraVectors();
//	}
//
//	glm::mat4 GetViewMatrix() {
//		return glm::lookAt(Position, Position + Front, Up);
//	}
//
//	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
//	{
//		float velocity = MovementSpeed * deltaTime;
//		if (direction == FORWARD)
//			Position += Front * velocity;
//		if (direction == BACKWARD)
//			Position -= Front * velocity;
//		if (direction == LEFT)
//			Position -= Right * velocity;
//		if (direction == RIGHT)
//			Position += Right * velocity;
//		if (direction == UP)
//			Position += Up * velocity;
//		if (direction == DOWN)
//			Position -= Up * velocity;
//	}
//
//	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
//	{
//		xoffset *= MouseSensitivity;
//		yoffset *= MouseSensitivity;
//
//		Yaw += xoffset;
//		Pitch += yoffset;
//
//		if (constrainPitch)
//		{
//			if (Pitch > 89.0f)
//				Pitch = 89.0f;
//			if (Pitch < -89.0f)
//				Pitch = -89.0f;
//		}
//
//		updateCameraVectors();
//	}
//
//	void ProcessMouseScroll(float yoffset)
//	{
//		Zoom -= (float)yoffset;
//		if (Zoom < 1.0f)
//			Zoom = 1.0f;
//		if (Zoom > 45.0f)
//			Zoom = 45.0f;
//	}
//
//	// New LookAt function
//	void LookAt(const glm::vec3& targetPos) {
//		Front = glm::normalize(targetPos - Position); // Set Front vector to point towards the target
//		Right = glm::normalize(glm::cross(Front, WorldUp)); // Recalculate Right vector
//		Up = glm::normalize(glm::cross(Right, Front));   // Recalculate Up vector
//	}
//
//
//
//	void Update(float dt) {}
//
//	glm::mat4 GetViewMatrix() const
//	{
//		return glm::lookAt(m_position, m_lookAt, glm::vec3(0, 1, 0));
//	}
//
//	glm::mat4 GetProjectionMatrix() const
//	{
//		glm::vec2 windowSize = Application::Get().GetWindowSize();
//		return glm::perspective(glm::radians(m_fov), (float)windowSize.x / (float)windowSize.y, 0.1f, 100.0f);
//	}
//
//	void CameraControl() {
//		/*BanKEngine::GlfwGlad::camera.Position = B_lerpVec3(BanKEngine::GlfwGlad::camera.Position, CameraHolder->Transform.getWorldPosition(), Time.Deltatime * 8);
//		CurrentLookat = B_lerpVec3(CurrentLookat, CameraLookat->Transform.getWorldPosition(), Time.Deltatime * 12);
//		BanKEngine::GlfwGlad::camera.LookAt(CurrentLookat);
//		BanKEngine::GlfwGlad::FOV = B_clamp(B_normalize(BackWheel.AngularVelocity * BackWheel.AngularVelocity * 0.02f, BackWheel.AngularVelocityMax * 0.01f, BackWheel.AngularVelocityMax), 0, 1) * 20 + 35;*/
//	}
//
//private:
//	void updateCameraVectors()
//	{
//		glm::vec3 front;
//		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
//		front.y = sin(glm::radians(Pitch));
//		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
//		Front = glm::normalize(front);
//		Right = glm::normalize(glm::cross(Front, WorldUp));
//		Up = glm::normalize(glm::cross(Right, Front));
//	}
//};
//
