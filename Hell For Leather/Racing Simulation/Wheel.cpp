#include "Wheel.h"

Wheel::Wheel(const glm::vec3& offsetPos,bool isLeftWheel) :
    offset(offsetPos),
    rotation(0.0f),
    steeringAngle(0.0f),
    maxSteeringAngle(45.0f),
    modelMatrix(glm::mat4(1.0f)),isLeft(isLeftWheel) {}

void Wheel::updateModelMatrix(const glm::mat4& carModelMatrix,const glm::vec3 scale,bool isSteeringWheel) {

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = carModelMatrix;
    modelMatrix = glm::translate(modelMatrix, offset);

    if (isLeft) {
        modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    }
    if (isSteeringWheel) {
        float adjustedSteeringAngle = isLeft ? -steeringAngle : steeringAngle;
        modelMatrix = glm::rotate(modelMatrix, glm::radians(adjustedSteeringAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, scale);
}

glm::mat4 Wheel::getModelMatrix() const {
    return modelMatrix;
}

void Wheel::setOffset(const glm::vec3& newOffset) {
	offset = newOffset;
}

// Optional: Function to set the rotation of the wheel
void Wheel::setRotation(float newRotation) {
    rotation = newRotation;
}

// Optional: Function to set the steering angle of the wheel
void Wheel::setSteeringAngle(float newSteeringAngle) {
    steeringAngle = glm::clamp(newSteeringAngle, -maxSteeringAngle, maxSteeringAngle);
}
