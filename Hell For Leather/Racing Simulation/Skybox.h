#pragma once

#include <vector>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Skybox {
public:
    Skybox(const std::vector<std::string>& faces, unsigned int shaderProg);
    ~Skybox();

    void load();
    void draw(glm::mat4 view, glm::mat4 projection);

private:
    unsigned int cubemapTexture;
    unsigned int skyboxVAO, skyboxVBO;
    unsigned int shaderProgram;  // Ensure this is declared
    std::vector<std::string> faces;

    unsigned int loadCubemap(const std::vector<std::string>& faces); // Updated to match implementation
};