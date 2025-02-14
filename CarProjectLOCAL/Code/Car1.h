#pragma once

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/model.h>

#include "Math.h"
#include "StaticObject.h"













//   This is temporary for test
///////////////////////////////////////////////////////////////////////
class InputEvent;
vector <InputEvent*> SystemInputList;
class InputEvent {

public:
    InputEvent(int GLFW_Key)
        :My_GLFW_Key(GLFW_Key) {
        SystemInputList.reserve(500);
        SystemInputList.push_back(this);
    }
    int My_GLFW_Key;
    bool Pressed = false;
    bool OnPressed = false;
    bool OnReleased = false;

    virtual void UpdateState(GLFWwindow* window) = 0;
    void UpdateEvent(int glfGet) {
        if (glfGet == GLFW_PRESS) {

            if (Pressed) {
                OnPressed = false;
            }
            else {
                OnPressed = true;
            }
            Pressed = true;
            OnReleased = false;
        }
        else {

            if (!Pressed) {
                OnReleased = false;
            }
            else {
                OnReleased = true;
            }
            Pressed = false;
            OnPressed = false;
        }
    }
};


namespace BanK_SystemKeys {

    class KeyEvent : public InputEvent {

    public:
        KeyEvent(int GLFW_Key)
            :InputEvent(GLFW_Key) {}

        void UpdateState(GLFWwindow* window) {
            UpdateEvent(glfwGetKey(window, My_GLFW_Key));
        }
    };

    KeyEvent A = KeyEvent(GLFW_KEY_A);
    KeyEvent B = KeyEvent(GLFW_KEY_B);
    KeyEvent C = KeyEvent(GLFW_KEY_C);
    KeyEvent D = KeyEvent(GLFW_KEY_D);
    KeyEvent E = KeyEvent(GLFW_KEY_E);
    KeyEvent F = KeyEvent(GLFW_KEY_F);
    KeyEvent G = KeyEvent(GLFW_KEY_G);
    KeyEvent H = KeyEvent(GLFW_KEY_H);
    KeyEvent I = KeyEvent(GLFW_KEY_I);
    KeyEvent J = KeyEvent(GLFW_KEY_J);
    KeyEvent K = KeyEvent(GLFW_KEY_K);
    KeyEvent L = KeyEvent(GLFW_KEY_L);
    KeyEvent M = KeyEvent(GLFW_KEY_M);
    KeyEvent N = KeyEvent(GLFW_KEY_N);
    KeyEvent O = KeyEvent(GLFW_KEY_O);
    KeyEvent P = KeyEvent(GLFW_KEY_P);
    KeyEvent Q = KeyEvent(GLFW_KEY_Q);
    KeyEvent R = KeyEvent(GLFW_KEY_R);
    KeyEvent S = KeyEvent(GLFW_KEY_S);
    KeyEvent T = KeyEvent(GLFW_KEY_T);
    KeyEvent U = KeyEvent(GLFW_KEY_U);
    KeyEvent V = KeyEvent(GLFW_KEY_V);
    KeyEvent W = KeyEvent(GLFW_KEY_W);
    KeyEvent X = KeyEvent(GLFW_KEY_X);
    KeyEvent Y = KeyEvent(GLFW_KEY_Y);
    KeyEvent Z = KeyEvent(GLFW_KEY_Z);

    KeyEvent Escape = KeyEvent(GLFW_KEY_ESCAPE);
    KeyEvent Tab = KeyEvent(GLFW_KEY_TAB);
    KeyEvent Space = KeyEvent(GLFW_KEY_SPACE);
    KeyEvent Backspace = KeyEvent(GLFW_KEY_BACKSPACE);
    KeyEvent Enter = KeyEvent(GLFW_KEY_ENTER);
    KeyEvent LeftShift = KeyEvent(GLFW_KEY_LEFT_SHIFT);
    KeyEvent RightShift = KeyEvent(GLFW_KEY_RIGHT_SHIFT);
    KeyEvent LeftCtrl = KeyEvent(GLFW_KEY_LEFT_CONTROL);
    KeyEvent RightCtrl = KeyEvent(GLFW_KEY_RIGHT_CONTROL);
    KeyEvent LFT_Alt = KeyEvent(GLFW_KEY_LEFT_ALT);
    KeyEvent RHT_Alt = KeyEvent(GLFW_KEY_RIGHT_ALT);
    KeyEvent CapsLock = KeyEvent(GLFW_KEY_CAPS_LOCK);
    KeyEvent F1 = KeyEvent(GLFW_KEY_F1);
    KeyEvent F2 = KeyEvent(GLFW_KEY_F2);
    KeyEvent F3 = KeyEvent(GLFW_KEY_F3);
    KeyEvent F4 = KeyEvent(GLFW_KEY_F4);
    KeyEvent F5 = KeyEvent(GLFW_KEY_F5);
    KeyEvent F6 = KeyEvent(GLFW_KEY_F6);
    KeyEvent F7 = KeyEvent(GLFW_KEY_F7);
    KeyEvent F8 = KeyEvent(GLFW_KEY_F8);
    KeyEvent F9 = KeyEvent(GLFW_KEY_F9);
    KeyEvent F10 = KeyEvent(GLFW_KEY_F10);
    KeyEvent F11 = KeyEvent(GLFW_KEY_F11);
    KeyEvent F12 = KeyEvent(GLFW_KEY_F12);
    KeyEvent LFT_BRACKET = KeyEvent(GLFW_KEY_LEFT_BRACKET);
    KeyEvent RHT_BRACKET = KeyEvent(GLFW_KEY_RIGHT_BRACKET);
    KeyEvent BACKSLASH = KeyEvent(GLFW_KEY_BACKSLASH);
    KeyEvent SLASH = KeyEvent(GLFW_KEY_SLASH);
    KeyEvent Equal = KeyEvent(GLFW_KEY_EQUAL);
    KeyEvent Minus = KeyEvent(GLFW_KEY_MINUS);


    void Update(GLFWwindow* window) {

        glfwPollEvents();

        for (InputEvent* pInst : SystemInputList) {
            pInst->UpdateState(window);
        }
    }
};












const glm::mat4 mat4one = glm::mat4(1.0f);
float KeyframeScale = 0.08;


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
        m_position = keyframes[0].position;
        m_rotation = keyframes[0].rotation;
        m_scale = keyframes[0].scale;
        m_forward = keyframes[0].forward;
        All_Ghost.push_back(this);
    }

    void Update(GLFWwindow* window, float dt); 
     

    Model m_model;
    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_scale;
    glm::vec3 m_forward 
        //= glm::vec3(0.0f, 0.0f, 1.0f)
        ;


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



glm::vec3 TargetPos = glm::vec3(0);
glm::vec3 TargetRot = glm::vec3(0);
glm::vec3 TargetFWD = glm::vec3(0);
void CarGhost::Update(GLFWwindow* window, float dt) {



    // Handle input for restart and play/pause
    if (BanK_SystemKeys::R.OnPressed) {
        currentTime = 0.0f;          // Reset the timer
        currentKeyframeIndex = 0;    // Start at the first keyframe
        isPlaying = true;
    }

    // If paused, exit early
    if (!isPlaying) return;

    // Check if there are keyframes to process
    if (keyframes.empty() || currentKeyframeIndex >= keyframes.size() - 1) {
        return;
    }

    currentTime += dt;

    if (currentKeyframeIndex > keyframes.size()-1) { currentKeyframeIndex = 0; isPlaying = false; }
    Keyframe currentKeyframe = keyframes[currentKeyframeIndex];
    Keyframe nextKeyframe = keyframes[currentKeyframeIndex + 1];
     
    float LerpSpeed = dt * 4;
    m_position = LerpVec3(m_position, TargetPos, LerpSpeed);
    m_rotation = TargetRot;
    m_forward = LerpVec3(m_forward, TargetFWD, LerpSpeed);

    if (currentTime > KeyframeScale) {
        TargetPos = nextKeyframe.position;
        TargetRot = nextKeyframe.rotation;
        TargetFWD = nextKeyframe.forward;
        m_scale = nextKeyframe.scale;

        currentKeyframeIndex++;
        currentTime = 0;
    }
}






