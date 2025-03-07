#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>

class Application
{
public:
    Application();
    virtual ~Application();

    void ProcessInput();
    void SwapBuffers();

    bool WindowShouldClose()
    {
        return glfwWindowShouldClose(window);
    }

    static Application& Get()
    {
        return *s_instance;
    }

    glm::vec2 GetWindowSize();
    glm::vec2 GetCursorPosition();

    void SetCursorEnable(bool value)
    {
        glfwSetInputMode(window, GLFW_CURSOR, value ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }

    GLFWwindow* GetWindow()
    {
        return window;
    }

private:
    static Application* s_instance;

    GLFWwindow* window = nullptr;
};