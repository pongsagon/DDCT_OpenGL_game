#pragma once

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/model.h>

#include "Math.h"
#include "StaticObject.h"
#include "Input.h"

float KeyframeScale = 0.2f;

struct Keyframe {
    float time;       
    glm::vec3 position;
    glm::vec3 scale;  
    glm::vec3 rotation;  
    glm::vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f);

    Keyframe(float t, glm::vec3 pos, glm::vec3 rot, glm::vec3 scl, glm::vec3 forward)
        : time(t), position(pos),rotation(rot), scale(scl),forward(forward) {}
};



class CarGhost;
vector <CarGhost*> All_Ghost;
class CarGhost  {
public:
    CarGhost(Model _model,vector<Keyframe> kf)
        : m_model(_model), currentTime(0.0f), currentKeyframeIndex(0)
    {
        keyframes = kf;
        //All_Ghost.push_back(this);
    }

    void Start()
    {
        if (keyframes.size() > 0)
        {
            currentTime = 0.0f;          // Reset the timer
            currentKeyframeIndex = 0;    // Start at the first keyframe
            isPlaying = true;

            m_position = keyframes[0].position;
            m_rotation = keyframes[0].rotation;
            m_scale = keyframes[0].scale;
            m_forward = keyframes[0].forward;
        }
    }

    void ClearKeyframes()
    {
        keyframes.clear();
    }

    void Update(float dt); 
 

    Model m_model;
    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_scale;
    glm::vec3 m_forward 
        //= glm::vec3(0.0f, 0.0f, 1.0f)
        ;

    void RenderKeyframe(Shader& shader, const Keyframe& key)
    {
        glm::mat4 modelRotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1.0f, 0));

        glm::vec3 pos = key.position;
        pos.y -= 0.5f;

        glm::mat4 rotation = glm::mat4(1.0f);

        // Apply rotations in Yaw -> Pitch -> Roll order
        rotation = glm::rotate(rotation, GetYawFromForward(key.forward), glm::vec3(0, 1, 0));   // Yaw (Y-axis)
        rotation = glm::rotate(rotation, key.rotation.x, glm::vec3(1, 0, 0)); // Pitch (X-axis)
        rotation = glm::rotate(rotation, key.rotation.z, glm::vec3(0, 0, 1));  // Roll (Z-axis)

        glm::mat4 model = glm::translate(glm::mat4(1.0f), pos)
            * rotation
            * modelRotation
            * glm::scale(glm::mat4(1.0f), m_scale);

        shader.setMat4("model", model);
        m_model.Draw(shader);
    }

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
    }

private:
    std::vector<Keyframe> keyframes; // Reference to the keyframes
    float currentTime;                // Tracks the elapsed time for interpolation
    bool isPlaying = false;                   // Play/pause state
    int currentKeyframeIndex;         // Index of the current keyframe
};

float Lerp(float start, float end, float t) {
    return start + t * (end - start);
}

glm::vec3 LerpVec3(glm::vec3 start, glm::vec3 end, float t) {
    return glm::vec3(Lerp(start.x, end.x, t),
        Lerp(start.y, end.y, t),
        Lerp(start.z, end.z, t));
}

glm::vec3 SlerpEuler(const glm::vec3& startEuler, const glm::vec3& endEuler, float t)
{
    glm::quat slerpedQuat = glm::slerp(glm::quat(startEuler), glm::quat(endEuler), t);
    return glm::eulerAngles(slerpedQuat);
}

void CarGhost::Update(float dt)
{
    if (Input::GetKeyDown(GLFW_KEY_R)){
        Start();
    }

    // If paused, exit early
    if (!isPlaying || keyframes.empty()) return;

    currentTime += dt;

    Keyframe currentKeyframe = keyframes[currentKeyframeIndex];
    Keyframe nextKeyframe = keyframes[currentKeyframeIndex];

    if (currentKeyframeIndex < keyframes.size() - 1)
        nextKeyframe = keyframes[currentKeyframeIndex + 1];
     
    float t = currentTime / KeyframeScale;
    m_position = LerpVec3(currentKeyframe.position, nextKeyframe.position, t);
    m_rotation = SlerpEuler(currentKeyframe.rotation, nextKeyframe.rotation, t);
    m_forward = SlerpEuler(currentKeyframe.forward, nextKeyframe.forward, t);

    if (currentTime >= KeyframeScale)
    {
        currentKeyframeIndex++;
        currentTime = 0;

        if (currentKeyframeIndex > keyframes.size() - 1)
            isPlaying = false;
    }
}






