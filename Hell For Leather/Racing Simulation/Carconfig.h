#pragma once

#include <glm/glm.hpp>

struct CarConfig {
    glm::vec3 position = glm::vec3(0.0f, 2.0f, 0.0f);
    glm::vec3 startPosition = glm::vec3(0.0f, 2.0f, 0.0f);
    glm::vec3 bodyOffset = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 bodyScale = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 wheelScale = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 direction = glm::vec3(0.0f, 0.0f, 1.0f);
    float rotation = 0.0f;
    float speed = 0.0f;
    float maxSpeed = 80.0f;
    float acceleration = 5.0f;
    float brakingForce = 20.0f;
    glm::vec3 frontLeftWheelOffset = glm::vec3(-0.65f, -0.6f, 0.85f);
    glm::vec3 frontRightWheelOffset = glm::vec3(0.65f, -0.6f, 0.85f);
    glm::vec3 backLeftWheelOffset = glm::vec3(-0.65f, -0.6f, -0.85f);
    glm::vec3 backRightWheelOffset = glm::vec3(0.65f, -0.6f, -0.85f);
    float maxSteeringAngleAtMaxSpeed = 15.0f;  // Maximum steering angle at maximum speed in degrees
    float maxSteeringAngleAtZeroSpeed = 45.0f;  // Maximum steering angle at zero speed in degrees
    float turnSharpnessFactor = 1.5f;
    float carWeight = 1000.0f;
    float calculateSteeringLimit() const {
        // Linearly interpolate between max angles based on the current speed
        float speedRatio = std::min(speed / maxSpeed, 1.0f);
        return glm::mix(maxSteeringAngleAtZeroSpeed, maxSteeringAngleAtMaxSpeed, speedRatio);
    }

    CarConfig() {}
};