#include "Car.h"

const float SHARP_TURN_SPEED_THRESHOLD = 50.0f; // speed in km/h
const float SHARP_TURN_ANGLE_THRESHOLD = 30.0f; // angle in degrees
float currentPitch;
float currentRoll;
// Define additional attributes for pitch control
float pitchControl = 0.0f;  // Delta change for pitch
const float PITCH_CONTROL_SPEED = 0.05f;  // Speed at which pitch is adjusted
const float MAX_PITCH_ANGLE = 10.0f;  // Maximum pitch angle in degre

bool isAirborne = false;
float verticalVelocity = 0.0f;
float forwardVelocity = 0.0f;  // Forward velocity for jumping
const float baseGravity = 9.8f;
const float jumpThresholdSpeed = 5.0f;
const float pitchJumpThreshold = 0.1f;
float verticalVelocityFactor = 0.1f;

Car::Car(const CarConfig& config)
    : position(config.position), startPosition(config.startPosition), bodyOffset(config.bodyOffset), bodyScale(config.bodyScale), direction(config.direction), rotation(config.rotation),
    speed(config.speed), maxSpeed(config.maxSpeed), acceleration(config.acceleration), maxSteeringAngleAtMaxSpeed(config.maxSteeringAngleAtMaxSpeed),
    maxSteeringAngleAtZeroSpeed(config.maxSteeringAngleAtZeroSpeed), turnSharpnessFactor(config.turnSharpnessFactor),
    brakingForce(config.brakingForce), wheelScale(config.wheelScale), frontLeftWheel(config.frontLeftWheelOffset, true),
    frontRightWheel(config.frontRightWheelOffset), backLeftWheel(config.backLeftWheelOffset, true),
    backRightWheel(config.backRightWheelOffset), carWeight(config.carWeight), modelMatrix(glm::mat4(1.0f)) {}


void Car::applyConfig(const CarConfig& config) {
    position = config.position;
    startPosition = config.startPosition;
    bodyScale = config.bodyScale;
    bodyOffset = config.bodyOffset;
    wheelScale = config.wheelScale;
    direction = config.direction;
    rotation = config.rotation;
    speed = config.speed;
    maxSpeed = config.maxSpeed;
    carWeight = config.carWeight;
    acceleration = config.acceleration;
    brakingForce = config.brakingForce;
    maxSteeringAngleAtMaxSpeed = config.maxSteeringAngleAtMaxSpeed;
    maxSteeringAngleAtZeroSpeed = config.maxSteeringAngleAtZeroSpeed;
    turnSharpnessFactor = config.turnSharpnessFactor;
    frontLeftWheel.setOffset(config.frontLeftWheelOffset);
    frontRightWheel.setOffset(config.frontRightWheelOffset);
    backLeftWheel.setOffset(config.backLeftWheelOffset);
    backRightWheel.setOffset(config.backRightWheelOffset);

    initializeModelMatrix();

}

void Car::activate() {
    active = true;
}

void Car::deactivate() {
    active = false;
}


void Car::startSelectionRotation() {
    rotatingForSelection = true;
}

void Car::stopSelectionRotation() {
    rotatingForSelection = false;
}

void Car::rotateForSelection(float deltaTime) {
    if (rotatingForSelection) {
        rotation += 30.0f * deltaTime; // Rotate 30 degrees per second
        rotation = fmod(rotation, 360.0f); // Keep the rotation within 0-360 degrees
    }
}

void Car::resetRotation() {
    rotation = 0.0f; // Resets the rotation to a default value, usually 0 degrees
    initializeModelMatrix(); // Reinitialize the model matrix with the updated rotation
}

bool Car::isActive() const {
    return active;
}
void Car::setCollisionGrid(const std::vector<std::vector<Triangle>>& gridCells, const std::vector<std::vector<Triangle>>& gridCellsCollision, float gridSize, int gridWidth, int gridHeight) {
    collisionChecker.setGrid(gridCells, gridCellsCollision, gridSize, gridWidth, gridHeight);
}

void Car::update(float deltaTime) {

    if (rotatingForSelection) {
        rotateForSelection(deltaTime);
    }

    if (!active) return;  // Skip updating if the car is not active or in selection mode


    updatePositionAndDirection(deltaTime);
    updateModelMatrix(deltaTime);
    updateWheelRotations(deltaTime);

}

void Car::initializeModelMatrix() {
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, bodyScale);  // Ensure the car's body scale is applied

    frontLeftWheel.updateModelMatrix(modelMatrix, wheelScale, true);
    frontRightWheel.updateModelMatrix(modelMatrix, wheelScale, true);
    backLeftWheel.updateModelMatrix(modelMatrix, wheelScale, false);
    backRightWheel.updateModelMatrix(modelMatrix, wheelScale, false);

}

void Car::updateModelMatrix(float deltaTime) {
    sideCollisionAABB.update(modelMatrix);

    bool sideCollision = collisionChecker.checkTrackIntersectionWithGrid(sideCollisionAABB);

    if (sideCollision) {
        glm::vec3 correctionDirection = (speed >= 0.0f) ? -direction : direction;
        glm::vec3 correction = correctionDirection * 0.3f;
        position += correction;
        speed *= 0.5f;
    }
    else {
        position = nextPosition;
    }

    // Offsets for wheel rays (pointing downward)
    glm::vec3 frontLeftWheelOffset = glm::vec3(-0.65f, 1.2f, 0.85f);
    glm::vec3 frontRightWheelOffset = glm::vec3(0.65f, 1.2f, 0.85f);
    glm::vec3 backLeftWheelOffset = glm::vec3(-0.65f, 1.2f, -0.85f);
    glm::vec3 backRightWheelOffset = glm::vec3(0.65f, 1.2f, -0.85f);

    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));

    frontLeftWheelRayOrigin = position + glm::vec3(rotationMatrix * glm::vec4(frontLeftWheelOffset, 1.0f));
    frontRightWheelRayOrigin = position + glm::vec3(rotationMatrix * glm::vec4(frontRightWheelOffset, 1.0f));
    backLeftWheelRayOrigin = position + glm::vec3(rotationMatrix * glm::vec4(backLeftWheelOffset, 1.0f));
    backRightWheelRayOrigin = position + glm::vec3(rotationMatrix * glm::vec4(backRightWheelOffset, 1.0f));

    bool frontLeftCollision = collisionChecker.checkTrackIntersectionWithGrid(frontLeftWheelRayOrigin, downwardRayDirection, frontLeftWheelIntersection);
    bool frontRightCollision = collisionChecker.checkTrackIntersectionWithGrid(frontRightWheelRayOrigin, downwardRayDirection, frontRightWheelIntersection);
    bool backLeftCollision = collisionChecker.checkTrackIntersectionWithGrid(backLeftWheelRayOrigin, downwardRayDirection, backLeftWheelIntersection);
    bool backRightCollision = collisionChecker.checkTrackIntersectionWithGrid(backRightWheelRayOrigin, downwardRayDirection, backRightWheelIntersection);

    bool wheelsTouchingGround = frontLeftCollision || frontRightCollision || backLeftCollision || backRightCollision;
    if (!wheelsTouchingGround && !isAirborne) {
        isAirborne = true;
        verticalVelocity = (speed * verticalVelocityFactor) / (carWeight / 1000.0f);  // Lower initial upward velocity for heavier cars
    }

    glm::vec3 midFront = (frontLeftWheelIntersection + frontRightWheelIntersection) / 2.0f;
    glm::vec3 midBack = (backLeftWheelIntersection + backRightWheelIntersection) / 2.0f;

    float rollHeightDifference = ((frontRightWheelIntersection.y + backRightWheelIntersection.y) / 2.0f) - ((frontLeftWheelIntersection.y + backLeftWheelIntersection.y) / 2.0f);
    float pitchHeightDifference = ((frontLeftWheelIntersection.y + frontRightWheelIntersection.y) / 2.0f) - ((backLeftWheelIntersection.y + backRightWheelIntersection.y) / 2.0f);

    float pitchAngleTarget = glm::atan(-pitchHeightDifference / glm::length(midFront - midBack));
    float rollAngleTarget = glm::atan(rollHeightDifference / glm::length(frontRightWheelIntersection - frontLeftWheelIntersection));

    float orientationLerpFactor = glm::mix(0.2f, 0.05f, glm::clamp(speed / maxSpeed, 0.0f, 1.0f));


    float targetY = (frontLeftWheelIntersection.y + frontRightWheelIntersection.y + backLeftWheelIntersection.y + backRightWheelIntersection.y) / 4.0f + 1.5f;

    if (isAirborne) {
        float adjustedGravity = baseGravity * (carWeight / 1000.0f);  // Heavier cars fall faster
        verticalVelocity -= adjustedGravity * deltaTime;
        position.y += verticalVelocity * deltaTime;
        currentPitch -= 0.1 * deltaTime;
        if (position.y <= targetY) {
            position.y = targetY;
            isAirborne = false;
            verticalVelocity = 0.0f;
        }
    }
    else {

        position.y = targetY;

        currentPitch = glm::mix(currentPitch, pitchAngleTarget, orientationLerpFactor);
        currentRoll = glm::mix(currentRoll, rollAngleTarget, orientationLerpFactor);

        if (speed > jumpThresholdSpeed && pitchAngleTarget > pitchJumpThreshold) {
            isAirborne = true;
            verticalVelocity = (speed * verticalVelocityFactor) / (carWeight / 1000.0f);  // Heavier cars launch with less velocity
        }
    }

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, currentPitch, glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, currentRoll, glm::vec3(0.0f, 0.0f, 1.0f));

    frontLeftWheel.updateModelMatrix(modelMatrix, wheelScale, true);
    frontRightWheel.updateModelMatrix(modelMatrix, wheelScale, true);
    backLeftWheel.updateModelMatrix(modelMatrix, wheelScale, false);
    backRightWheel.updateModelMatrix(modelMatrix, wheelScale, false);
}


glm::mat4 Car::getModelMatrix() const {
    glm::mat4 bodyModelMatrix = glm::translate(modelMatrix, bodyOffset);
    bodyModelMatrix = glm::scale(bodyModelMatrix, bodyScale);
    return bodyModelMatrix;
}

// Accessors for wheel matrices
glm::mat4 Car::getFrontLeftWheelModelMatrix() const {
    return frontLeftWheel.getModelMatrix();
}

glm::mat4 Car::getFrontRightWheelModelMatrix() const {
    return frontRightWheel.getModelMatrix();
}

glm::mat4 Car::getBackLeftWheelModelMatrix() const {
    return backLeftWheel.getModelMatrix();
}

glm::mat4 Car::getBackRightWheelModelMatrix() const {
    return backRightWheel.getModelMatrix();
}


// Getters for position, direction, speed, and rotation
glm::vec3 Car::getPosition() const {
    return position;
}

glm::vec3 Car::getDirection() const {
    return direction;
}

float Car::getSpeed() const {
    return speed;
}

float Car::getRotation() const {
    return rotation;
}

float Car::getMaxSpeed() const {
    return maxSpeed;
}


void Car::accelerate(float deltaTime) {
    if (!isAirborne) {
        // Only allow acceleration if the car is on the ground
        speed += acceleration * deltaTime;
        if (speed > maxSpeed) {
            speed = maxSpeed;
        }
    }

}


void Car::brake(float deltaTime) {
    if (!isAirborne) {
        speed -= brakingForce * deltaTime;
        if (speed < -maxSpeed / 2.0f) {
            speed = -maxSpeed / 2.0f;  // Limit reverse speed
        }
    }

}

void Car::slowDown(float deltaTime) {

    if (speed > 0) {
        speed -= acceleration * deltaTime;
        if (speed < 0) speed = 0; // Stop the car when speed reaches 0
    }
    else if (speed < 0) {
        speed += acceleration * deltaTime;
        if (speed > 0) speed = 0; // Stop the car when reverse speed reaches 0
    }

}

bool Car::isSharpTurn(float steeringAngle) const {
    // Define a sharp turn as having a large steering angle at a high speed
    // You could make this more sophisticated by making the threshold speed-dependent
    return std::abs(steeringAngle) > SHARP_TURN_ANGLE_THRESHOLD && speed > SHARP_TURN_SPEED_THRESHOLD;
}

void Car::steerLeft(float deltaTime) {

    float angleChange = 120.0f * turnSharpnessFactor * deltaTime; // Degrees per second
    frontLeftWheel.steeringAngle += angleChange;
    frontRightWheel.steeringAngle += angleChange;

    // Clamp the steering angle
    frontLeftWheel.steeringAngle = glm::clamp(frontLeftWheel.steeringAngle, -45.0f, 45.0f);
    frontRightWheel.steeringAngle = glm::clamp(frontRightWheel.steeringAngle, -45.0f, 45.0f);

    // Check if it's a sharp turn
    bool sharpTurn = isSharpTurn(frontLeftWheel.steeringAngle);
    if (sharpTurn) {
        // Adjust handling for sharp turn, e.g., reduce speed
        slowDown(deltaTime * 2); // Slow down faster if it's a sharp turn
    }
}

void Car::steerRight(float deltaTime) {
    float angleChange = -120.0f * turnSharpnessFactor * deltaTime; // Degrees per second
    frontLeftWheel.steeringAngle += angleChange;
    frontRightWheel.steeringAngle += angleChange;

    // Clamp the steering angle
    frontLeftWheel.steeringAngle = glm::clamp(frontLeftWheel.steeringAngle, -45.0f, 45.0f);
    frontRightWheel.steeringAngle = glm::clamp(frontRightWheel.steeringAngle, -45.0f, 45.0f);

    // Check if it's a sharp turn
    bool sharpTurn = isSharpTurn(frontRightWheel.steeringAngle);
    if (sharpTurn) {
        // Adjust handling for sharp turn
        slowDown(deltaTime * 2); // Slow down faster if it's a sharp turn
    }
}

// Gradually center the steering and update the wheel direction accordingly
void Car::centerSteering(float deltaTime) {
    frontLeftWheel.steeringAngle = glm::mix(frontLeftWheel.steeringAngle, 0.0f, 2.0f * deltaTime);
    frontRightWheel.steeringAngle = glm::mix(frontRightWheel.steeringAngle, 0.0f, 2.0f * deltaTime);
}

float Car::getSteeringAngle() const {
    // Assuming you have some logic to compute or retrieve the steering angle
    // For example, if steering angle is simply the average of both front wheels:
    return (frontLeftWheel.steeringAngle + frontRightWheel.steeringAngle) / 2.0f;
}

void Car::updatePositionAndDirection(float deltaTime) {

    // Update the car's direction based on the rotation (yaw)
    direction = glm::vec3(sin(glm::radians(rotation)), 0.0f, cos(glm::radians(rotation)));

    // If the car is moving, update its rotation
    if (speed != 0.0f) {
        rotation += glm::clamp(frontLeftWheel.steeringAngle, -45.0f, 45.0f) * deltaTime;

        // Predict the next position
        nextPosition = position + direction * speed * deltaTime;
    }
    else {
        nextPosition = position;  // No movement if the car is not moving
    }

}


void Car::moveToStartPosition() {
    position = startPosition;  // startPosition should be part of CarConfig or stored in Car
    initializeModelMatrix();   // Update model matrix to reflect new position
}

void Car::updateWheelRotations(float deltaTime) {

    float rotationSpeed = speed * deltaTime * 360.0f;

    backLeftWheel.rotation -= rotationSpeed;
    backRightWheel.rotation += rotationSpeed;
    frontLeftWheel.rotation -= rotationSpeed;
    frontRightWheel.rotation += rotationSpeed;

}