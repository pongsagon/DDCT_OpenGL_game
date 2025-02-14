#pragma once

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/model.h>

#include "Math.h"
#include "StaticObject.h"
#include "Audio.h"
#include "Input.h"

class Car
{
public:
    Car(const std::string& modelPath, const glm::vec3& position, const glm::vec3& scale)
        : m_model(modelPath)
        , m_position(position)
        , m_scale(scale)
        , m_velocity{ 0 }
        , m_forward({ 0.0f, 0.0f, 1.0f })
        , m_rotation{ 0 }
        , m_wheelModel("Assets/Models/car/wheel.obj")
    {
    }

    glm::vec3 GetPosition() const
    {
        return m_position;
    }

    void SetPosition(const glm::vec3& position)
    {
        m_position = position;
    }

    glm::vec3 GetScale() const
    {
        return m_scale;
    }

    void SetScale(const glm::vec3& scale)
    {
        m_scale = scale;
    }

    void Update(float dt)
    {
        int accelerationInput = 0;
        int steerInput = 0;

        if (Input::GetKey(GLFW_KEY_W))
            accelerationInput = 1;

        if (Input::GetKey(GLFW_KEY_S))
            accelerationInput = -1;

        if (Input::GetKey(GLFW_KEY_D))
        {
            steerInput = 1;
            m_steerTimer += dt;
        }
        else if (Input::GetKey(GLFW_KEY_A))
        {
            steerInput = -1;
            m_steerTimer += dt;
        }
        else
        {
            m_steerPowerCounter -= 1;
            m_steerTimer = 0.0f;
        }

        if (m_steerTimer >= m_steerUpgradeTime)
        {
            m_steerPowerCounter += 1;
            m_steerTimer = 0.0f;
        }

        if (m_steerPowerCounter > m_maxSteerPower)
            m_steerPowerCounter = m_maxSteerPower;
        else if (m_steerPowerCounter < 0)
            m_steerPowerCounter = 0;

        float steerAngle = steerInput * m_steerFactor;

        glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(steerAngle), glm::vec3(0, 1, 0));
        glm::vec3 newForward = rot * glm::vec4{ m_forward.x, m_forward.y, m_forward.z, 0.0f };

        float amount = glm::exp(m_steerLerpFactor * m_steerPowerCounter);
        m_forward = glm::normalize(Lerp(m_forward, newForward, amount * dt));

        float newfrontWheelAngle = m_frontWheelAngle;
        newfrontWheelAngle += glm::clamp(amount * glm::radians(steerAngle), -4.0f * dt, 4.0f * dt);
        newfrontWheelAngle += glm::clamp(-m_frontWheelAngle, -2.0f * dt, 2.0f * dt);
        m_frontWheelAngle = glm::clamp(newfrontWheelAngle, -0.68f, 0.68f);

        glm::vec3 acceleration = m_forward * (accelerationInput * m_accelerationFactor);

        glm::vec3 forward = glm::normalize(m_forward); // Use your existing forward vector
        glm::vec3 up = glm::vec3(0, 1, 0); // World up vector (y-axis)
        glm::vec3 right = glm::normalize(glm::cross(up, forward));

        glm::vec3 lateral_velocity = right * dot(m_velocity, right);
        glm::vec3 lateral_friction = -lateral_velocity * lateral_friction_factor;
        glm::vec3 backwards_friction = -m_velocity * backward_friction_factor;

        m_velocity += (backwards_friction + lateral_friction) * dt;

        float current_speed = glm::length(m_velocity);

        steerAngle *= current_speed / m_maxVelocity;

        if (current_speed < m_maxVelocity)
        {
            m_velocity += acceleration * dt;
        }

        if (glm::dot(m_forward, m_velocity) < 0.0f)
            current_speed *= -1;

        m_frontWheelsRotation.x += current_speed * dt;

        float angle = glm::acos(glm::dot(m_forward, { 0, 0, 1 }));
        glm::vec3 crossProduct = glm::cross({ 0, 0, 1 }, m_forward);
        if (crossProduct.y < 0) {
            angle = -angle;
        }

        m_frontWheelsRotation.y = angle;

        m_position += m_velocity * dt;
    }

    bool CheckRayCollisionWithObject(const Ray& ray, std::vector<Triangle>& triangles, glm::vec3& intersectionPoint)
    {
        glm::vec3 hitPoint;
        float minDistance = std::numeric_limits<float>::max();

        // Loop through all triangles in the scene
        for (const Triangle& triangle : triangles) {
            float distance = RayIntersectTriangle(ray, triangle, hitPoint);

            if (distance < minDistance) {
                minDistance = distance;
                intersectionPoint = hitPoint;
            }
        }

        return minDistance < std::numeric_limits<float>::max();
    }

    void CheckCollisions(const std::vector<StaticObject>& objects, float dt)
    {
        Ray carRay{ m_position, glm::vec3(0, -1, 0) };

        Ray frontRay{ m_position, glm::vec3(0, -1, 0) };
        frontRay.position += m_frontWheelOffset * m_forward;

        Ray backRay{ m_position, glm::vec3(0, -1, 0) };
        backRay.position -= m_frontWheelOffset * m_forward;

        glm::vec3 right = glm::normalize(glm::cross(m_forward, glm::vec3{ 0.0f, 1.0f, 0.0f }));

        Ray leftRay{ m_position, glm::vec3(0, -1, 0) };
        leftRay.position -= m_sideWheelOffset * right;

        Ray rightRay{ m_position, glm::vec3(0, -1, 0) };
        rightRay.position += m_sideWheelOffset * right;

        for (const auto& object : objects)
        {
            auto transformedTriangles = object.GetNearTriangles(m_position, 100.0f);

            {
                glm::vec3 frontIntersectionPoint, backIntersectionPoint;
                glm::vec3 leftIntersectionPoint, rightIntersectionPoint;

                bool frontHit = CheckRayCollisionWithObject(frontRay, transformedTriangles, frontIntersectionPoint);
                bool backHit = CheckRayCollisionWithObject(backRay, transformedTriangles, backIntersectionPoint);
                bool leftHit = CheckRayCollisionWithObject(leftRay, transformedTriangles, leftIntersectionPoint);
                bool rightHit = CheckRayCollisionWithObject(rightRay, transformedTriangles, rightIntersectionPoint);

                int count = 0;
                float yTotal = 0.0f;

                if (frontHit)
                {
                    count++;
                    yTotal += frontIntersectionPoint.y;
                }


                if (backHit)
                {
                    count++;
                    yTotal += backIntersectionPoint.y;
                }

                if (leftHit)
                {
                    count++;
                    yTotal += leftIntersectionPoint.y;
                }

                if (rightHit)
                {
                    count++;
                    yTotal += rightIntersectionPoint.y;
                }

                if (backHit && frontHit)
                {
                    glm::vec3 kVector = backIntersectionPoint - frontIntersectionPoint;

                    float yDiff = backIntersectionPoint.y - frontIntersectionPoint.y;

                    glm::vec3 newRot = m_rotation;
                    newRot.x = glm::atan(yDiff / (m_frontWheelOffset * 2));
                    glm::vec3 actualRot = Lerp(m_rotation, newRot, dt * 10.0f);

                    m_rotation.x = actualRot.x;
                }
                else
                {
                    m_rotation.x = 0.0f;
                }

                if (leftHit && rightHit)
                {
                    float yDiff = leftIntersectionPoint.y - rightIntersectionPoint.y;

                    m_rotation.z = glm::clamp(glm::atan(yDiff / (m_sideWheelOffset * 2)), -1.0f, 1.0f);

                    glm::vec3 newRot = m_rotation;
                    newRot.z = glm::atan(yDiff / (m_frontWheelOffset * 2));
                    glm::vec3 actualRot = Lerp(m_rotation, newRot, dt * 10.0f);

                    m_rotation.z = actualRot.z;
                }
                else
                {
                    m_rotation.z = 0.0f;
                }

                if (count > 0)
                {
                    m_position.y = 0.52f + (yTotal / (float)count);
                }
            }

        }

    }

    void Render(Shader& shader)
    {
        //glm::mat4 modelRotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1.0f, 0));
        //glm::mat4 modelRotation = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));
        glm::mat4 modelRotation = glm::mat4(1.0f);

        glm::vec3 pos = m_position;
        pos.y -= 0.5f;

        glm::mat4 rotation = glm::mat4(1.0f);

        // Apply rotations in Yaw -> Pitch -> Roll order
        rotation = glm::rotate(rotation, GetYawFromForward(m_forward), glm::vec3(0, 1, 0));   // Yaw (Y-axis)
        rotation = glm::rotate(rotation, m_rotation.x, glm::vec3(1, 0, 0)); // Pitch (X-axis)
        rotation = glm::rotate(rotation, m_rotation.z, glm::vec3(0, 0, 1));  // Roll (Z-axis)

        glm::mat4 model = glm::translate(glm::mat4(1.0f), pos)
            * rotation
            * modelRotation
            * glm::scale(glm::mat4(1.0f), m_scale);

        shader.setMat4("model", model);
        shader.setMat4("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
        m_model.Draw(shader);

        glm::vec3 frontWheelPosition = m_forward * m_frontWheelOffset;
        glm::vec3 backWheelPosition = m_position - m_forward * m_frontWheelOffset;

        float wheelZOffset = 0.8f;
        glm::vec3 rightWheelScale = m_scale;
        rightWheelScale.z *= -1;

        glm::mat4 frontLeftWheelModel = model * glm::translate(glm::mat4(1.0f), glm::vec3(-0.875, 0.3f, wheelZOffset))
            * glm::rotate(glm::mat4(1.0f), m_frontWheelAngle, glm::vec3(0, 1, 0))
            * glm::rotate(glm::mat4(1.0f), m_frontWheelsRotation.x, glm::vec3(0, 0, 1))
            * glm::scale(glm::mat4(1.0f), m_scale);

        glm::mat4 frontRightWheelModel = model * glm::translate(glm::mat4(1.0f), glm::vec3(-0.875, 0.3f, -wheelZOffset))
            * glm::rotate(glm::mat4(1.0f), m_frontWheelAngle, glm::vec3(0, 1, 0))
            * glm::rotate(glm::mat4(1.0f), m_frontWheelsRotation.x, glm::vec3(0, 0, 1))
            * glm::scale(glm::mat4(1.0f), rightWheelScale);

        shader.setMat4("model", frontLeftWheelModel);
        m_wheelModel.Draw(shader);

        shader.setMat4("model", frontRightWheelModel);
        m_wheelModel.Draw(shader);

        glm::mat4 backLeftWheelModel = model * glm::translate(glm::mat4(1.0f), glm::vec3(1.475, 0.3f, wheelZOffset))
            * glm::rotate(glm::mat4(1.0f), m_frontWheelsRotation.x, glm::vec3(0, 0, 1))
            * glm::scale(glm::mat4(1.0f), m_scale);

        glm::mat4 backrightWheelModel = model * glm::translate(glm::mat4(1.0f), glm::vec3(1.475, 0.3f, -wheelZOffset))
            * glm::rotate(glm::mat4(1.0f), m_frontWheelsRotation.x, glm::vec3(0, 0, 1))
            * glm::scale(glm::mat4(1.0f), rightWheelScale);

        shader.setMat4("model", backLeftWheelModel);
        m_wheelModel.Draw(shader);

        shader.setMat4("model", backrightWheelModel);
        m_wheelModel.Draw(shader);
    }

    float audioTime = 0;
    irrklang::ISound* currentSound = nullptr;
    float engineRound;

    void AudioUpdate(Audio& audio, float dt, GLFWwindow* window)
    {
        float audioWait = 1 / (6 + engineRound * 0.2f);
        audioWait = std::abs(audioWait);

        if (audioTime > audioWait)
        {
            audioTime = 0;

            if (currentSound = audio.PlaySound("CarEngine"))
            {
                currentSound->setPlaybackSpeed(engineRound * 0.065f + 1.35f); // Example: Increase pitch by 50%
                currentSound->setVolume(0.5f);
            }
        }
        else
        {
            audioTime += dt;
        }

        if (currentSound)
        {
            if (Input::GetKey(GLFW_KEY_W))
            {
                currentSound->setVolume(1.0f);
                engineRound += dt * 70.0f;
            }
            else if (Input::GetKey(GLFW_KEY_S))
                engineRound += dt * 10.0f;
            else
                engineRound = Lerpf(engineRound, 0, dt * 0.55f);

            engineRound = glm::clamp(engineRound, 0.0f, 45.0f);
        }
    }

private:
    friend class FollowCamera;

    glm::vec3 m_frontWheelsRotation{ 0.0f };

    float m_frontWheelOffset = 0.45f;
    float m_sideWheelOffset = 0.25f;

    Model m_wheelModel;

    float m_accelerationFactor = 12.0f;
    float m_steerFactor = -35.0f;
    float m_steerLerpFactor = 0.06f;
    float lateral_friction_factor = 3.75f;
    float backward_friction_factor = 0.22f;

    float m_maxVelocity = 50.0f;

    Model m_model;
    glm::vec3 m_velocity;
    glm::vec3 m_forward;
    glm::vec3 m_rotation;
    glm::vec3 m_bodyRotation;

    float m_bodyRotationX = 0.0f;
    float collisionThreshold = 0.5f; // Threshold for collision detection
    float collisionResponseFactor = 0.5f; // Factor to adjust collision response strength
    float collisionDampingFactor = 0.3f; // Damping factor to reduce velocity after collision

    int m_steerPowerCounter = 0;
    int m_maxSteerPower = 20;
    float m_steerTimer = 0.0f;
    float m_steerUpgradeTime = 0.015f;

    float m_frontWheelAngle = 0.0f;

    glm::vec3 m_scale;
    glm::vec3 m_position;
};
