#include "Application.h"
#include "Input.h"

const static unsigned int SCR_WIDTH = 1200;
const static unsigned int SCR_HEIGHT = 600;

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    Input::MouseMoved(xposIn, yposIn);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        Input::Press(key);
    }
    else if (action == GLFW_RELEASE)
    {
        Input::Release(key);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        Input::MousePress(button);
    }
    else if (action == GLFW_RELEASE)
    {
        Input::MouseRelease(button);
    }
}

Application* Application::s_instance = nullptr;

Application::Application()
{
    s_instance = this;

    // glfw: initialize and configure
// ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }
}

Application::~Application()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

glm::vec2 Application::GetWindowSize()
{
    glm::ivec2 windowSize{ 0 };
    //glfwGetWindowSize(window, &windowSize.x, &windowSize.y);
    glfwGetFramebufferSize(window, &windowSize.x, &windowSize.y);

    return windowSize;
}

glm::vec2 Application::GetCursorPosition()
{
    glm::dvec2 cursorPos{ 0 };
    glfwGetCursorPos(window, &cursorPos.x, &cursorPos.y);

    return cursorPos;
}


void Application::ProcessInput()
{
    Input::PrepareKeyStates();
    glfwPollEvents();

    if (Input::GetKey(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, true);
}

void Application::SwapBuffers()
{
    glfwSwapBuffers(window);
}
