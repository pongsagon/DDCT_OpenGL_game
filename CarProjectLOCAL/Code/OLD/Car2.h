#pragma once
#include "Car1.h"





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
        , m_wheelModel("Assets/Models/lambo/lambo_Wheels.obj")
    {
    }

    glm::vec3 hitPoint;
    float minDistance;
    float R_distance;


    glm::vec3 GetPosition() const
    {
        return m_position;
    }

    void Update(GLFWwindow* window, float dt)
    {
        PlaceKeyframes(dt); 
        if (BanK_SystemKeys::E.OnPressed) {
            All_Ghost.clear();


            if (!keyframes.empty()) {
                CarGhost* NewGhost = new CarGhost(m_model, keyframes);
                keyframes.clear(); 
            }
        }




        int accelerationInput = 0;
        int steerInput = 0;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            accelerationInput = 1;

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            accelerationInput = -1;

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            steerInput = 1;
            m_steerTimer += dt;
        }
        else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
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

        float amount = dt * glm::exp(m_steerLerpFactor * m_steerPowerCounter);
        m_forward = glm::normalize(Lerp(m_forward, newForward, amount));

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

        m_frontWheelsRotation.x += current_speed * dt;

        float angle = glm::acos(glm::dot(m_forward, { 0, 0, 1 }));
        glm::vec3 crossProduct = glm::cross({ 0, 0, 1 }, m_forward);
        if (crossProduct.y < 0) {
            angle = -angle;
        }

        m_frontWheelsRotation.y = angle;

        m_position += m_velocity * dt * 10.0f;
    }

    bool CheckRayCollisionWithObject(const Ray& ray, std::vector<Triangle>& triangles, glm::vec3& intersectionPoint)
    {
        minDistance = std::numeric_limits<float>::max();

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

    float frontWheelOffset = 0.45f;
    float sideWheelOffset = 0.25f;


    std::vector<Keyframe> keyframes; // Vector of keyframes
    float timeAccumulator = 0.0f;
    float timeKey = 0.0f;
    void PlaceKeyframes(float Deltatime) { 
        timeAccumulator += Deltatime;
        timeKey += Deltatime;

        // Check if we have reached the 0.2-second interval
        if (timeAccumulator > KeyframeScale) {
            // Create a keyframe with the current position and scale
            Keyframe newKeyframe(timeKey, m_position, m_rotation, m_scale,m_forward);

            // Add the keyframe to the vector
            keyframes.push_back(newKeyframe); 

            // Reset the time accumulator
            timeAccumulator = 0.0f;
        }
    }
    void CheckCollisions(const std::vector<StaticObject>& objects, float dt)
    {
        glm::vec3 Raypos = m_position;
        Raypos.y += 4;

        Ray carRay{ Raypos, glm::vec3(0, -1, 0) };

        Ray frontRay{ Raypos, glm::vec3(0, -1, 0) };
        frontRay.position += frontWheelOffset * m_forward;

        Ray backRay{ Raypos, glm::vec3(0, -1, 0) };
        backRay.position -= frontWheelOffset * m_forward;

        glm::vec3 right = glm::normalize(glm::cross(m_forward, glm::vec3{ 0.0f, 1.0f, 0.0f }));

        Ray leftRay{ Raypos, glm::vec3(0, -1, 0) };
        leftRay.position -= sideWheelOffset * right;

        Ray rightRay{ Raypos, glm::vec3(0, -1, 0) };
        rightRay.position += sideWheelOffset * right;



        for (const auto& object : objects)
        {
            auto transformedTriangles = object.GetNearTriangles(m_position, 100.0f);

            glm::vec3 intersectionPoint;

            //if (CheckRayCollisionWithObject(carRay, transformedTriangles, intersectionPoint))
            {
                //m_position.y = 1.0f + intersectionPoint.y;

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
                    newRot.x = glm::atan(yDiff / (frontWheelOffset * 2));
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

                    m_rotation.z = glm::clamp(glm::atan(yDiff / (sideWheelOffset * 2)), -1.0f, 1.0f);

                    glm::vec3 newRot = m_rotation;
                    newRot.z = glm::atan(yDiff / (frontWheelOffset * 2));
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

    float m_bodyRotationX = 0.0f;

    float collisionThreshold = 0.5f; // Threshold for collision detection
    float collisionResponseFactor = 0.5f; // Factor to adjust collision response strength
    float collisionDampingFactor = 0.3f; // Damping factor to reduce velocity after collision

    void Render(Shader& shader)
    {
        glm::mat4 modelRotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1.0f, 0));

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
        m_model.Draw(shader);

        glm::vec3 frontWheelPosition = m_forward * frontWheelOffset;
        glm::vec3 backWheelPosition = m_position - m_forward * frontWheelOffset;

        glm::mat4 wheelModel = model * glm::translate(glm::mat4(1.0f), glm::vec3(-0.9, 0.3f, 0))
            //* glm::mat4(glm::quat(m_frontWheelsRotation))
            * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0, 1.0f, 0))
            * glm::rotate(glm::mat4(1.0f), m_frontWheelsRotation.x, glm::vec3(1.0f, 0, 0))
            * glm::scale(glm::mat4(1.0f), 0.95f * m_scale);

        shader.setMat4("model", wheelModel);

        m_wheelModel.Draw(shader);

        /*

        model = glm::translate(glm::mat4(1.0f), m_position)
            //* glm::mat4(glm::quat(m_frontWheelsRotation))
            * rotation
            * rotationMatrix
            * modelRotation
            * glm::scale(glm::mat4(1.0f), m_scale);


        shader.setMat4("model", model);

        m_wheelModel.Draw(shader);
        */
    }

private:
    friend class FollowCamera;

    glm::vec3 m_frontWheelsRotation{ 0.0f };

    Model m_wheelModel;

    float m_accelerationFactor = 9.0f;
    float m_steerFactor = -30.0f;
    float m_steerLerpFactor = 0.06f;
    float lateral_friction_factor = 4.5f;
    float backward_friction_factor = 0.22f;

    float m_maxVelocity = 45.0f;

    Model m_model;
    glm::vec3 m_velocity;
    glm::vec3 m_forward;
    glm::vec3 m_rotation;
    glm::vec3 m_bodyRotation;

    int m_steerPowerCounter = 0;
    int m_maxSteerPower = 20;
    float m_steerTimer = 0.0f;
    float m_steerUpgradeTime = 0.015f;

    glm::vec3 m_scale;
    glm::vec3 m_position;
};
