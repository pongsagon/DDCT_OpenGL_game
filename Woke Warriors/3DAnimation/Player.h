#pragma once

#include "camera.h"
#include "animator.h"
#include "model_animation.h"
#include <iostream>
#include <map>

struct BoxCollider {
    glm::vec3 center;
    glm::vec3 size;
};

enum AnimState {
    IDLE,
    WALK,
    PUNCH,
    KICK
};

class Player {
public:
    Model model;
    Animator animator;
    std::map<std::string, Animation> animations;
    BoxCollider collider;
    glm::vec3 position;
    AnimState state;
    std::string currentAnimation;

    Player(const std::string& modelPath, const glm::vec3& pos, const glm::vec3& colliderSize, const std::map<std::string, std::string>& animationPaths);

    void loadAnimations(const std::map<std::string, std::string>& animationPaths);
    void Action(const std::string& action, bool blend);
    void updateAnimation(float deltaTime);
    void setAnimation(const std::string& animName);
    void update(float deltaTime);
    std::vector<glm::mat4>GetFinalBoneMatrices();
    void draw(Shader& shader);
    void processInput(int key);
    void changeState(AnimState newState);
};
