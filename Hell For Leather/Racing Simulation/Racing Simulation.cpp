#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "shader_m.h"
#include "Skybox.h"
#include "camera.h"
#include "model.h"
#include "irrKlang/irrKlang.h"

#include "Car.h" 
#include "Carconfig.h"
#include "SoundManager.h"
#include "Timer.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

void renderScene(Shader& shader);
void processInput(GLFWwindow* window);

void handleCarSound(SoundManager& soundManager, const Car& car);

float calculateOptimalGridSize(const Model& trackModel, int desiredGridCount);
void assignTrianglesToGrid(const Model& trackModel, float gridSize, int gridWidth, int gridHeight, std::vector<std::vector<Triangle>>& gridCells);
void checkTrackSize(const Model& trackModel);
void renderCube();
void renderQuad();

void renderUIQuad();
unsigned int loadTexture(const char* path);

void initTextRendering(const std::string& fontPath);
void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color);



// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 8.5f, -55.0f));
float near_plane = 0.1f, far_plane = 200.0f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

glm::vec3 rayOrigin;
glm::vec3 rayDirection = glm::vec3(0.0f, -1.0f, 0.0f);

//track divided into 15x15 grid with each block being 20x20 in size
int gridWidth = 8;
int gridHeight = 8;
float gridSize = 0.0f;

CarConfig chevConfig;
CarConfig cadillacConfig;
Car chev(chevConfig);
Car cadillac(cadillacConfig);
Car* selectedCar = &chev;

std::vector<std::vector<Triangle>> gridCells;
std::vector<std::vector<Triangle>> gridCellsCollision;

Model* trackVisual;
Model* carModel;
Model* wheelModel;
Model* car2Model;
Model* wheel2Model;

SoundManager soundManager;

bool gameStarted = false;
glm::vec3 minBounds(-3.0f, -2.0f, -2.0f);
glm::vec3 maxBounds(3.0f, 2.0f, 2.0f);

Timer timer(minBounds, maxBounds);


glm::vec3 lightPositions[4] = {
glm::vec3(10.0f, 5.0f, 10.0f),
glm::vec3(-10.0f, 5.0f, 10.0f),
glm::vec3(10.0f, 5.0f, -10.0f),
glm::vec3(-10.0f, 5.0f, -10.0f)
};

glm::vec3 lightColors[4] = {
    glm::vec3(500.0f, 500.0f, 500.0f),
    glm::vec3(500.0f, 500.0f, 500.0f),
    glm::vec3(500.0f, 500.0f, 500.0f),
    glm::vec3(500.0f, 500.0f, 500.0f)
};

// Character struct similar to your existing setup
struct Character {
    unsigned int TextureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing; 
    unsigned int Advance;
};

std::map<GLchar, Character> Characters;
unsigned int textVAO, textVBO;



int main()
{

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Racing Game", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(false);


    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    // set depth function to less than AND equal for skybox depth trick.
    glDepthFunc(GL_LEQUAL);
    // enable seamless cubemap sampling for lower mip levels in the pre-filter map.
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("Shaders/model/model_loading.vs", "Shaders/model/model_loading.fs");
    Shader pbrShader("Shaders/PBR/pbr.vs", "Shaders/PBR/pbr.fs");
    Shader equirectangularToCubemapShader("Shaders/PBR/cubemap.vs", "Shaders/PBR/equirectangular_to_cubemap.fs");
    Shader irradianceShader("Shaders/PBR/cubemap.vs", "Shaders/PBR/irradiance_convolution.fs");
    Shader prefilterShader("Shaders/PBR/cubemap.vs", "Shaders/PBR/prefilter.fs");
    Shader brdfShader("Shaders/PBR/brdf.vs", "Shaders/PBR/brdf.fs");
    Shader backgroundShader("Shaders/PBR/background.vs", "Shaders/PBR/background.fs");
    Shader uiShader("Shaders/UIShader.vs", "Shaders/UIShader.fs");
    Shader textShader("Shaders/text.vs", "Shaders/text.fs");

    // load models
    // -----------
    Shader skyboxShader("Shaders/skybox/skybox.vs", "Shaders/skybox/skybox.fs");
    std::vector<std::string> faces = {
    "Textures/skybox/sunset/px.jpg",
    "Textures/skybox/sunset/nx.jpg",
    "Textures/skybox/sunset/py.jpg",
    "Textures/skybox/sunset/ny.jpg",
    "Textures/skybox/sunset/pz.jpg",
    "Textures/skybox/sunset/nz.jpg"
    };

    /*Shader skyboxShader("Shaders/skybox/skybox.vs", "Shaders/skybox/skybox.fs");
    std::vector<std::string> faces = {
    "Textures/sunset/px.png",
    "Textures/sunset/nx.png",
    "Textures/sunset/py.png",
    "Textures/sunset/ny.png",
    "Textures/sunset/pz.png",
    "Textures/sunset/nz.png"
    };*/

    Skybox skybox(faces, skyboxShader.getID());
    Model trackModel("Objects/racetrack/track.obj");
    Model trackCollisionModel("Objects/racetrack/trackCol.obj");


    trackVisual = new Model("Objects/racetrack/track3.obj");
    carModel = new Model("Objects/chev-nascar/body.obj");
    wheelModel = new Model("Objects/chev-nascar/wheel1.obj");
    car2Model = new Model("Objects/pbrCar/CarBody2.obj");
    wheel2Model = new Model("Objects/pbrCar/carwheel.obj");
    //Model carModel("Objects/jeep/car.obj");
    //Model wheelModel("Objects/jeep/wheel.obj");
    //Model carModel("Objects/chev-nascar/body.obj");
    //Model wheelModel("Objects/chev-nascar/wheel1.obj");

    unsigned int uiTexture = loadTexture("Textures/UI/square.png");
    glm::mat4 uiProjection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));

    uiShader.use();
    uiShader.setMat4("projection", uiProjection);
    uiShader.setInt("uiTexture", 0);

    ourShader.use();
    ourShader.setInt("material.diffuse", 0);
    ourShader.setInt("material.specular", 1);

    pbrShader.use();
    pbrShader.setInt("irradianceMap", 0);
    pbrShader.setInt("prefilterMap", 1);
    pbrShader.setInt("brdfLUT", 2);
    pbrShader.setInt("albedoMap", 3);
    pbrShader.setInt("normalMap", 4);
    pbrShader.setInt("metallicMap", 5);
    pbrShader.setInt("roughnessMap", 6);
    pbrShader.setInt("aoMap", 7);

    backgroundShader.use();
    backgroundShader.setInt("environmentMap", 0);

    initTextRendering("Textures/Fonts/digital-7.ttf");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
    textShader.use();
    glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // pbr: setup framebuffer
   // ----------------------
    unsigned int captureFBO;
    unsigned int captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    // pbr: load the HDR environment map
    // ---------------------------------
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float* data = stbi_loadf("Textures/newport_loft.hdr", &width, &height, &nrComponents, 0);
    unsigned int hdrTexture;
    if (data)
    {
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        std::cout << "HDR Loaded: Width = " << width << ", Height = " << height << ", Components = " << nrComponents << std::endl;

        stbi_image_free(data);

    }
    else
    {
        std::cout << "Failed to load HDR image." << std::endl;
    }

    // pbr: setup cubemap to render to and attach to framebuffer
    // ---------------------------------------------------------
    unsigned int envCubemap;
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
    // ----------------------------------------------------------------------------------------------
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))

    };

    // pbr: convert HDR equirectangular environment map to cubemap equivalent
    // ----------------------------------------------------------------------
    equirectangularToCubemapShader.use();
    equirectangularToCubemapShader.setInt("equirectangularMap", 0);
    equirectangularToCubemapShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        equirectangularToCubemapShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderCube();

    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
    // --------------------------------------------------------------------------------
    unsigned int irradianceMap;
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    // pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
    // -----------------------------------------------------------------------------
    irradianceShader.use();
    irradianceShader.setInt("environmentMap", 0);
    irradianceShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        irradianceShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderCube();

    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
    // --------------------------------------------------------------------------------
    unsigned int prefilterMap;
    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minification filter to mip_linear 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
    // ----------------------------------------------------------------------------------------------------
    prefilterShader.use();
    prefilterShader.setInt("environmentMap", 0);
    prefilterShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        // reisze framebuffer according to mip-level size.
        unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilterShader.setFloat("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            prefilterShader.setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderCube();
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // pbr: generate a 2D LUT from the BRDF equations used.
    // ----------------------------------------------------
    unsigned int brdfLUTTexture;
    glGenTextures(1, &brdfLUTTexture);

    // pre-allocate enough memory for the LUT texture.
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

    glViewport(0, 0, 512, 512);
    brdfShader.use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    chevConfig.position = glm::vec3(-3.0f, 10.0f, -53.0f);
    chevConfig.startPosition = glm::vec3(-2.5f, 0.0f, -1.5f);
    chevConfig.bodyOffset = glm::vec3(0.0f, -1.5f, 0.0f);
    chevConfig.bodyScale = glm::vec3(0.7f, 0.7f, 0.7f);
    chevConfig.wheelScale = glm::vec3(0.7f, 0.7f, 0.7f);
    chevConfig.carWeight = 1000.0f;
    chevConfig.maxSpeed = 30.0f;
    chevConfig.acceleration = 8.0f;
    chevConfig.brakingForce = 7.5f;
	chevConfig.turnSharpnessFactor = 1.0f;
    chevConfig.maxSteeringAngleAtMaxSpeed= 30.0f;
	chevConfig.maxSteeringAngleAtZeroSpeed = 45.0f;
    chevConfig.frontRightWheelOffset = glm::vec3(-0.8f, -1.2f, 1.50f);
    chevConfig.frontLeftWheelOffset = glm::vec3(0.8f, -1.2f, 1.50f);
    chevConfig.backRightWheelOffset = glm::vec3(-0.8f, -1.2f, -1.1f);
    chevConfig.backLeftWheelOffset = glm::vec3(0.8f, -1.2f, -1.1f);

    cadillacConfig.position = glm::vec3(3.0f, 10.0f,-57.0f);
    cadillacConfig.startPosition = glm::vec3(2.5f, 0.0f, -1.5f);
    cadillacConfig.bodyOffset = glm::vec3(0.0f, -1.5f, 0.0f);
    cadillacConfig.bodyScale = glm::vec3(0.5f, 0.5f, 0.5f);
    cadillacConfig.wheelScale = glm::vec3(0.4f, 0.4f, 0.4f);
    cadillacConfig.carWeight = 1300.0f;
    cadillacConfig.maxSpeed = 25.0f;
    cadillacConfig.acceleration = 4.0f;
    cadillacConfig.brakingForce = 3.0f;
	cadillacConfig.turnSharpnessFactor = 0.7f;
	cadillacConfig.maxSteeringAngleAtMaxSpeed = 10.0f;
	cadillacConfig.maxSteeringAngleAtZeroSpeed = 45.0f;

    cadillacConfig.frontRightWheelOffset = glm::vec3(-0.65f, -1.2f, 1.20f);
    cadillacConfig.frontLeftWheelOffset = glm::vec3(0.65f, -1.2f, 1.20f);
    cadillacConfig.backRightWheelOffset = glm::vec3(-0.65f, -1.2f, -1.20f);
    cadillacConfig.backLeftWheelOffset = glm::vec3(0.65f, -1.2f, -1.20f);

    gridSize = calculateOptimalGridSize(trackModel, gridHeight);

    chev.applyConfig(chevConfig);
    cadillac.applyConfig(cadillacConfig);
   
    selectedCar = &chev;
    camera.LookAtCar(chev.getPosition() - glm::vec3(0, 1.8f, 0));

    chev.startSelectionRotation();
    cadillac.startSelectionRotation();
  

    assignTrianglesToGrid(trackModel, gridSize, gridWidth, gridHeight, gridCells);
    assignTrianglesToGrid(trackCollisionModel, gridSize, gridWidth, gridHeight, gridCellsCollision);
    chev.setCollisionGrid(gridCells,gridCellsCollision, gridSize, gridWidth, gridHeight);
    cadillac.setCollisionGrid(gridCells, gridCellsCollision, gridSize, gridWidth, gridHeight);
    soundManager.preloadSound("accelerate", "Sounds/accelerate_sound2.wav");
    soundManager.preloadSound("music", "Sounds/Plasma.wav");
    soundManager.playSound("music", true);
	soundManager.setVolume("music", 0.5f);

    pbrShader.use();
    for (int i = 0; i < 4; ++i) {
        pbrShader.setVec3("lightPositions[" + std::to_string(i) + "]", lightPositions[i]);
        pbrShader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
    }


    int scrWidth, scrHeight;
    glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
    glViewport(0, 0, scrWidth, scrHeight);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {

        // per-frame time logic
        // --------------------

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);
       
        camera.FollowCar(selectedCar->getPosition(), selectedCar->getDirection(), selectedCar->getSpeed(), selectedCar->getMaxSpeed(), selectedCar->getSteeringAngle(), deltaTime);
        camera.CarPosition = selectedCar->getPosition();
        camera.Update(deltaTime);
        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        pbrShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, near_plane, far_plane);
        glm::mat4 view = camera.GetViewMatrix();
        pbrShader.setMat4("projection", projection);
        pbrShader.setMat4("view", view);
        pbrShader.setVec3("camPos", camera.Position);

        //track
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);


        renderScene(pbrShader);

        //Update car position and direction
        selectedCar->updatePositionAndDirection(deltaTime);
        selectedCar->updateModelMatrix(deltaTime);  // Update the car and wheel transformations

        if (!gameStarted) {
            chev.update(deltaTime);
            cadillac.update(deltaTime);

        }
        else {
            selectedCar->update(deltaTime); // Only update the selected car
        }

        //render skybox
        skybox.draw(view, projection);

        if (gameStarted) {
            // Update timer
            timer.update(selectedCar->getPosition());
            // Render the timer text
            std::string timerText = timer.getFormattedTime();
            std::string bestLapTimeText = "Best Lap: " + timer.getBestLapTime();
            RenderText(textShader, timerText, 10.0f, static_cast<float>(SCR_HEIGHT) - 50.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            RenderText(textShader, bestLapTimeText, 10.0f, static_cast<float>(SCR_HEIGHT) - 80.0f, 0.8f, glm::vec3(0.0f, 1.0f, 0.0f));
        }
        else
        {
            RenderText(textShader, "Press [1]/[2] to select car.", 10.0f, static_cast<float>(SCR_HEIGHT) - 50.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            RenderText(textShader, "Press [Enter] to confirm.", 10.0f, static_cast<float>(SCR_HEIGHT) - 80.0f, 0.8f, glm::vec3(0.0f, 1.0f, 0.0f));
        }
        handleCarSound(soundManager, chev);
        handleCarSound(soundManager, cadillac);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void renderScene(Shader& shader) {
    // Track
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", glm::transpose(glm::inverse(glm::mat3(model))));
    shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
    trackVisual->Draw(shader);

    // Render cars based on game state and activation
    if (!gameStarted || (gameStarted && chev.isActive())) {
        // Draw the Chevrolet car body
        shader.setMat4("model", chev.getModelMatrix());
        shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(chev.getModelMatrix()))));
        carModel->Draw(shader);  // Assuming carModel is the model for Chevrolet

        // Draw the Chevrolet wheels
        shader.setMat4("model", chev.getFrontLeftWheelModelMatrix());
        shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(chev.getFrontLeftWheelModelMatrix()))));
        wheelModel->Draw(shader);  // Assuming wheelModel is shared or change accordingly

        shader.setMat4("model", chev.getFrontRightWheelModelMatrix());
        shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(chev.getFrontRightWheelModelMatrix()))));
        wheelModel->Draw(shader);

        shader.setMat4("model", chev.getBackLeftWheelModelMatrix());
        shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(chev.getBackLeftWheelModelMatrix()))));
        wheelModel->Draw(shader);

        shader.setMat4("model", chev.getBackRightWheelModelMatrix());
        shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(chev.getBackRightWheelModelMatrix()))));
        wheelModel->Draw(shader);
    }

    if (!gameStarted || (gameStarted && cadillac.isActive())) {
        // Draw the Jeep car body
        shader.setMat4("model", cadillac.getModelMatrix());
        shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(cadillac.getModelMatrix()))));
        car2Model->Draw(shader);  // Assuming car2Model is the model for Jeep

        // Draw the Jeep wheels
        shader.setMat4("model", cadillac.getFrontLeftWheelModelMatrix());
        shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(cadillac.getFrontLeftWheelModelMatrix()))));
        wheel2Model->Draw(shader);  // Assuming wheel2Model is shared or change accordingly

        shader.setMat4("model", cadillac.getFrontRightWheelModelMatrix());
        shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(cadillac.getFrontRightWheelModelMatrix()))));
        wheel2Model->Draw(shader);

        shader.setMat4("model", cadillac.getBackLeftWheelModelMatrix());
        shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(cadillac.getBackLeftWheelModelMatrix()))));
        wheel2Model->Draw(shader);

        shader.setMat4("model", cadillac.getBackRightWheelModelMatrix());
        shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(cadillac.getBackRightWheelModelMatrix()))));
        wheel2Model->Draw(shader);
    }
}



void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !gameStarted) {
        selectedCar = &chev;
        camera.LookAtCar(chev.getPosition() - glm::vec3(0,1.0f,0));
    }
    else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !gameStarted) {
        selectedCar = &cadillac;
        camera.LookAtCar(cadillac.getPosition() - glm::vec3(0, 1.0f, 0));
    }

    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        selectedCar->stopSelectionRotation();
        selectedCar->moveToStartPosition();
        selectedCar->resetRotation();
        selectedCar->activate();
        if (selectedCar == &chev) {
            cadillac.stopSelectionRotation();
            cadillac.deactivate();
        }
        else {
            chev.stopSelectionRotation();
            chev.deactivate();
        }
        gameStarted = true;
        camera.shouldFollow = true;
    }

    // Acceleration and braking
    if (selectedCar && selectedCar->isActive() && gameStarted) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            selectedCar->accelerate(deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            selectedCar->brake(deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE) {
            selectedCar->slowDown(deltaTime);
        }

        // Steering
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            selectedCar->steerLeft(deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            selectedCar->steerRight(deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE) {
            selectedCar->centerSteering(deltaTime);
        }

        // Update car position and direction
        selectedCar->updatePositionAndDirection(deltaTime);
        selectedCar->updateWheelRotations(deltaTime);
    }
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (!gameStarted)
        return;
    static float mouse_lastX = SCR_WIDTH / 2.0;
    static float mouse_lastY = SCR_HEIGHT / 2.0;
    float xoffset = xpos - mouse_lastX;
    float yoffset = mouse_lastY - ypos; // Reverse since y-coordinates range from bottom to top
    mouse_lastX = xpos;
    mouse_lastY = ypos;

    // Check if the left mouse button is being held down
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}



void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (!gameStarted)
        return;
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            // When the left mouse button is pressed, start dragging
            camera.StartDragging();
        }
        else if (action == GLFW_RELEASE) {
            // When the left mouse button is released, stop dragging
            camera.StopDragging();
        }
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    static double lastX = xpos;
    static double lastY = ypos;

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    // Only update camera if dragging is active
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

// Function to be called whenever the mouse scroll wheel is used
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

glm::vec3 calculateTriangleNormal(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) {
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
    return normal;
}

void printGridCells() {

    int totalSum = 0;

    for (int i = 0; i < gridCells.size(); ++i) {

        int num = gridCells[i].size();  // Number of triangles in this cell
        std::cout << "Grid Cell " << i << std::endl;
        std::cout << "Number of triangles: " << num << std::endl;

        totalSum += num;
    }

    std::cout << "TOTAL SUM: " << totalSum << std::endl;
}


int getGridIndex(int x, int z, int gridWidth) {
    return z * gridWidth + x;
}


void assignTrianglesToGrid(const Model& trackModel, float gridSize, int gridWidth, int gridHeight, std::vector<std::vector<Triangle>>& gridCells) {

    // Resize gridCells
    gridCells.resize(gridWidth * gridHeight);

    for (const Mesh& mesh : trackModel.meshes) {
        for (unsigned int i = 0; i < mesh.indices.size(); i += 3) {

            glm::vec3 v0 = mesh.vertices[mesh.indices[i]].Position;
            glm::vec3 v1 = mesh.vertices[mesh.indices[i + 1]].Position;
            glm::vec3 v2 = mesh.vertices[mesh.indices[i + 2]].Position;

            // min and maxx and z coordinates of the triangle
            float minX = std::min({ v0.x, v1.x, v2.x });
            float maxX = std::max({ v0.x, v1.x, v2.x });
            float minZ = std::min({ v0.z, v1.z, v2.z });
            float maxZ = std::max({ v0.z, v1.z, v2.z });

            //grid cells that the triangle overlaps (how far way the point is from the origin in terms of grid cells)
            int minGridX = static_cast<int>(floor(minX / gridSize));
            int maxGridX = static_cast<int>(floor(maxX / gridSize));
            int minGridZ = static_cast<int>(floor(minZ / gridSize));
            int maxGridZ = static_cast<int>(floor(maxZ / gridSize));

            // Clamp grid coordinates to ensure they stay within the grid boundaries
            minGridX = std::max(0, std::min(gridWidth - 1, minGridX));
            maxGridX = std::max(0, std::min(gridWidth - 1, maxGridX));
            minGridZ = std::max(0, std::min(gridHeight - 1, minGridZ));
            maxGridZ = std::max(0, std::min(gridHeight - 1, maxGridZ));

            Triangle tri = { v0, v1, v2 };

            // Assign the triangle to the relevant grid cells
            for (int x = minGridX; x <= maxGridX; ++x) {
                for (int z = minGridZ; z <= maxGridZ; ++z) {
                    int index = getGridIndex(x, z, gridWidth);
                    gridCells[index].push_back(tri);
                }
            }
        }
    }

    //printGridCells();

}

void checkTrackSize(const Model& trackModel) {

    int minX = 0;
    int maxX = 0;

    for (const Mesh& mesh : trackModel.meshes) {
        for (unsigned int i = 0; i < mesh.indices.size(); i += 3) {

            glm::vec3 v0 = mesh.vertices[mesh.indices[i]].Position;
            glm::vec3 v1 = mesh.vertices[mesh.indices[i + 1]].Position;
            glm::vec3 v2 = mesh.vertices[mesh.indices[i + 2]].Position;

            if (v0.x < minX) minX = v0.x;
            if (v1.x < minX) minX = v1.x;
            if (v2.x < minX) minX = v2.x;

            if (v0.x > maxX) maxX = v0.x;
            if (v1.x > maxX) maxX = v1.x;
            if (v2.x > maxX) maxX = v2.x;

        }
    }

    std::cout << "Minimum X: " << minX << "Maximum Y" << maxX << std::endl;

}

void handleCarSound(SoundManager& soundManager, const Car& car) {
    static float fadeOutVolume = 1.0f;

    // Check if the car is moving forward
    if (car.getSpeed() > 0.0f) {
        // If the sound is not playing, play it from the beginning
        if (!soundManager.isPlaying("accelerate")) {
            soundManager.stopSound("accelerate");  // Ensure the sound is reset
            soundManager.playSound("accelerate", true);  // Play looped
            soundManager.setVolume("accelerate", 0.2f);  // Start with a low volume
        }

        // Adjust pitch and volume based on speed
        float pitch = 1.0f + (car.getSpeed() / car.getMaxSpeed());
        soundManager.setPlaybackSpeed("accelerate", pitch);

        float volume = glm::clamp(car.getSpeed() / car.getMaxSpeed(), 0.2f, 1.0f);
        soundManager.setVolume("accelerate", volume);

        fadeOutVolume = volume;
    }
    else {  // If the car is stopped
        if (fadeOutVolume > 0.0f) {
            fadeOutVolume -= deltaTime * 0.5f;
            fadeOutVolume = glm::clamp(fadeOutVolume, 0.0f, 1.0f);
            soundManager.setVolume("accelerate", fadeOutVolume);
        }
        else {
            soundManager.stopSound("accelerate");  // Stop the sound when fully faded out
        }
    }
}


float calculateOptimalGridSize(const Model& trackModel, int desiredGridCount) {

    // Initialize min and max bounds
    glm::vec3 minBounds(FLT_MAX, FLT_MAX, FLT_MAX);
    glm::vec3 maxBounds(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    for (const Mesh& mesh : trackModel.meshes) {
        for (unsigned int i = 0; i < mesh.indices.size(); i++) {

            glm::vec3 vertex = mesh.vertices[mesh.indices[i]].Position;

            minBounds = glm::min(minBounds, vertex);
            maxBounds = glm::max(maxBounds, vertex);

        }
    }

    // Calculate the dimensions of the bounding box for the entire track
    glm::vec3 trackSize = maxBounds - minBounds;

    // Find the maximum extent along the X and Z axes (for 2D grid division)
    float maxDimension = glm::max(trackSize.x, trackSize.z);

    // Calculate the optimal grid size based on the desired number of grids
    float gridSize = maxDimension / desiredGridCount;

    return gridSize;
}


// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
             // bottom face
             -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
              1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
              1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
              1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
             -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             // top face
             -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
              1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
              1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
              1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);


}


unsigned int uiVAO = 0;
unsigned int uiVBO;
void renderUIQuad() {
    if (uiVAO == 0) {
        float uiVertices[] = {
            // positions    // texture Coords
             1.0f,  1.0f,  1.0f, 1.0f, // Top Right
             1.0f, -1.0f,  1.0f, 0.0f, // Bottom Right
            -1.0f, -1.0f,  0.0f, 0.0f, // Bottom Left
            -1.0f,  1.0f,  0.0f, 1.0f  // Top Left 
        };

        // Setup UI VAO and VBO
        glGenVertexArrays(1, &uiVAO);
        glGenBuffers(1, &uiVBO);
        glBindVertexArray(uiVAO);
        glBindBuffer(GL_ARRAY_BUFFER, uiVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(uiVertices), &uiVertices, GL_STATIC_DRAW);

        // Position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        // Texture Coordinate attribute
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        // Unbind for safety
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    // Render UI Quad
    glBindVertexArray(uiVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // Drawing as TRIANGLE_FAN or TRIANGLE_STRIP
    glBindVertexArray(0);

}

unsigned int loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        std::cout << "Failed to load texture: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void initTextRendering(const std::string& fontPath) {
    // Initialize FreeType library
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return;
    }

    if (fontPath.empty())
    {
        std::cout << "ERROR::FREETYPE: Failed to load fontPath" << std::endl;
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
        std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
        FT_Done_FreeType(ft);
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

    // Load first 128 characters of ASCII
    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
            continue;
        }
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    // Clean up FreeType
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // Set up text VAO/VBO
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color) {

    glUseProgram(shader.ID); // Use text shader
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Ensure proper blending for text
    glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);

    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        x += (ch.Advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}