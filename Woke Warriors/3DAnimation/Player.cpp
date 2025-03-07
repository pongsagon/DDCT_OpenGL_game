#include "Player.h"

Player::Player(const std::string& modelPath, const glm::vec3& pos, const glm::vec3& colliderSize, const std::map<std::string, std::string>& animationPaths)
    : model(modelPath), position(pos), collider{ pos, colliderSize }, animator(nullptr) { // Assuming nullptr is acceptable initially
    model.loadModel(modelPath);
    loadAnimations(animationPaths);
    
}

void Player::loadAnimations(const std::map<std::string, std::string>& animationPaths) {
    for (const auto& anim : animationPaths) {
        animations[anim.first] = Animation(anim.second, &model);
    }
    animator = Animator(&animations.begin()->second); // Default to first animation
}

void Player::setAnimation(const std::string& animName) {
    if (animations.find(animName) != animations.end()) {
        animator.PlayAnimation(&animations[animName]);
    }
}

void Player::Action(const std::string& action, bool blend) {
    if (animations.find(action) == animations.end()) return; // Action not found

    if (blend && currentAnimation != action) {
        // Start blending from the current animation to the new action
        animator.PlayAnimation(&animations[currentAnimation], &animations[action], 0.0f, 0.0f, 0.1f);  // Adjust blend parameters as needed
    }
    else {
        // Switch directly to the new action without blending
        animator.PlayAnimation(&animations[action], nullptr, 0.0f, 0.0f, 0.0f);
    }
    currentAnimation = action;  // Update the current animation to the new action
}

void Player::updateAnimation(float deltaTime) {
    animator.UpdateAnimation(deltaTime);
}

void Player::update(float deltaTime) {
    animator.UpdateAnimation(deltaTime);
    // Additional update logic here
}

std::vector<glm::mat4> Player::GetFinalBoneMatrices() {
    return animator.GetFinalBoneMatrices();
}


void Player::draw(Shader& shader) {
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);
    shader.setMat4("model", modelMatrix);
    model.Draw(shader);  // Assuming model has a draw method
}

void Player::processInput(int key) {
    // Input handling logic here
}

void Player::changeState(AnimState newState) {
    if (state != newState) {
        state = newState;
        const std::string animName = [this]() {
            switch (state) {
            case WALK: return "walk";
            case PUNCH: return "punch";
            case KICK: return "kick";
            default: return "idle";
            }
            }();
        setAnimation(animName);
    }
}
