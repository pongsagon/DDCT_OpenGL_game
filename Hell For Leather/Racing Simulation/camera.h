#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    glm::vec3 CarPosition;  // Position of the car to follow
    float Yaw;
    float Pitch;
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    bool isDragging;
    float cameraLerpSpeed = 5.0f;
    float OrbitRadius;
    float OrbitMinRadius = 5.0f;
    float OrbitMaxRadius = 20.0f;
    bool shouldFollow;
    glm::vec3 TargetFront;
    bool isTransitioning;
    float transitionSpeed = 10.0f;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH, float orbitRadius = 10.0f)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM), OrbitRadius(orbitRadius), isDragging(false), shouldFollow(false) {  // Initialize shouldFollow to false
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix() {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void ProcessKeyboard(Camera_Movement direction, float deltaTime) {
        if (isDragging) return;  // Ignore keyboard input while dragging
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    void FollowCar(glm::vec3 carPosition, glm::vec3 carDirection, float carSpeed, float maxSpeed, float steeringAngle, float deltaTime) {
        if (isDragging || !shouldFollow) return;

    // Set follow distance directly without lerping
    float followDistance = glm::mix(3.0f, 5.5f, glm::clamp(carSpeed / maxSpeed, 0.0f, 1.0f));  // Closer positioning
    float baseCameraHeightOffset = 0.2f;  // Base height offset above the car

    // Dynamically adjust the camera height offset based on the car's elevation
    float dynamicCameraHeightOffset = carPosition.y + baseCameraHeightOffset;

    // Calculate the target position directly behind the car without lerping
    glm::vec3 targetPosition = carPosition - carDirection * followDistance + glm::vec3(0.0f, dynamicCameraHeightOffset, 0.0f);

    // Calculate the side offset based on steering angle and apply lerping only to this component
    glm::vec3 rightVector = glm::normalize(glm::cross(carDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
    float maxLateralOffset = 1.0f; // Reduced lateral movement
    float lateralOffset = (steeringAngle / 45.0f) * maxLateralOffset;

    // Only the lateral offset is smoothed using lerp
    glm::vec3 lateralPosition = targetPosition + rightVector * lateralOffset;
    Position.x = glm::mix(Position.x, lateralPosition.x, deltaTime * cameraLerpSpeed);
    Position.z = glm::mix(Position.z, lateralPosition.z, deltaTime * cameraLerpSpeed);
    Position.y = targetPosition.y;  // Adjust height directly based on car's position

    // Update camera vectors
    Front = glm::normalize(CarPosition - Position);
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));

}

    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) {
        if (!isDragging) return;

        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        if (constrainPitch) {
            Pitch = glm::clamp(Pitch, -89.0f, 89.0f);
        }

        updateCameraPosition();
    }

    void LookAtCar(glm::vec3 targetPosition) {
        TargetFront = glm::normalize(targetPosition - Position);
        isTransitioning = true;
    }


    void Update(float deltaTime) {
        if (isTransitioning) {
            Front = glm::normalize(glm::mix(Front, TargetFront, deltaTime * transitionSpeed));
            if (glm::length(Front - TargetFront) < 0.01) {
                Front = TargetFront;
                isTransitioning = false;
            }
            Right = glm::normalize(glm::cross(Front, WorldUp));
            Up = glm::normalize(glm::cross(Right, Front));
        }
    }

    void updateCameraPosition() {
        glm::vec3 offset;
        offset.x = OrbitRadius * cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        offset.y = OrbitRadius * sin(glm::radians(Pitch));
        offset.z = OrbitRadius * sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Position = CarPosition + offset;

        updateCameraVectors();
    }

    void StartDragging() {
        isDragging = true;
    }

    void StopDragging() {
        isDragging = false;
        updateCameraPosition(); // Ensure camera updates its position to follow the car when dragging stops
    }

    void ProcessMouseScroll(float yoffset) {
        OrbitRadius -= (float)yoffset;
        OrbitRadius = glm::clamp(OrbitRadius, OrbitMinRadius, OrbitMaxRadius);
        updateCameraPosition();
    }

private:
    void updateCameraVectors() {
        // Calculate the front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // Recalculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
};
#endif
