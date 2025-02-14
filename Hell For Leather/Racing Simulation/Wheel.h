#ifndef WHEEL_H
#define WHEEL_H

#include <glm/gtc/matrix_transform.hpp>

class Wheel {
public:
    Wheel(const glm::vec3& offsetPos, bool isLeftWheel = false);
    void updateModelMatrix(const glm::mat4& carModelMatrix, const glm::vec3 scale, bool isSteeringWheel);
    glm::mat4 getModelMatrix() const;
    float getSteeringAngle()
    {
        return steeringAngle;
    }

    void setRotation(float rotation);
    void setOffset(const glm::vec3& offset);
    void setSteeringAngle(float steeringAngle);

    float rotation;
    float steeringAngle;
    bool isLeft;

private:
    glm::vec3 offset;
    glm::vec3 scale;
    glm::mat4 modelMatrix;

    float maxSteeringAngle;

};

#endif

