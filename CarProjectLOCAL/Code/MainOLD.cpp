#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.h>
#include <learnopengl/model.h>
//#include <learnopengl/camera.h>

#include "Application.h"
#include "Car.h"
#include "FollowCamera.h"
#include "EditorCamera.h"
#include "Renderer.h"

#include <iostream>

unsigned int sphereVAO = 0;
unsigned int indexCount;

void RenderPbrScene(Shader& shader, Camera& camera);

void renderSphere()
{
    if (sphereVAO == 0)
    {
        glGenVertexArrays(1, &sphereVAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359f;
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                positions.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        indexCount = static_cast<unsigned int>(indices.size());

        std::vector<float> data;
        for (unsigned int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
        }
        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        unsigned int stride = (3 + 2 + 3) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    }

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}

void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
void renderScene(Renderer& renderer, const Shader& shader);
void renderQuad();

// settings
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

// camera
//Camera camera(glm::vec3(0.0f, 2.0f, 3.0f));

//Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

unsigned int albedo;
unsigned int normal;
unsigned int metallic;
unsigned int roughness;
unsigned int ao;

// meshes
unsigned int planeVAO;


#include "ImGuiManager.h"

int main()
{
    Application app;
    Renderer renderer;
    renderer.SetupPBR("Assets/Textures/hdr/mountains_sunset_sky_dome_1k.hdr");

    ImGuiManager imgui;

    Shader debugDepthQuad("Assets/Shaders/3.1.3.debug_quad.vs", "Assets/Shaders/3.1.3.debug_quad_depth.fs");


    Shader shader("Assets/Shaders/1.2.pbr.vs", "Assets/Shaders/1.2.pbr.fs");

    shader.use();
    shader.setInt("albedoMap", 0);
    shader.setInt("normalMap", 1);
    shader.setInt("metallicMap", 2);
    shader.setInt("roughnessMap", 3);
    shader.setInt("aoMap", 4);

    // load PBR material textures
    // --------------------------
    
          /*
    albedo = loadTexture("Assets/Textures/rusted_iron/albedo.png");
    normal = loadTexture("Assets/Textures/rusted_iron/normal.png");
    metallic = loadTexture("Assets/Textures/rusted_iron/metallic.png");
    roughness = loadTexture("Assets/Textures/rusted_iron/roughness.png");
    ao = loadTexture("Assets/Textures/rusted_iron/ao.png");


    albedo = loadTexture("Assets/Textures/foil/Foil002_1K-PNG_Color.png");
    normal = loadTexture("Assets/Textures/foil/Foil002_1K-PNG_NormalGL.png");
    metallic = loadTexture("Assets/Textures/foil/Foil002_1K-PNG_Metalness.png");
    roughness = loadTexture("Assets/Textures/foil/Foil002_1K-PNG_Roughness.png");
    ao = loadTexture("Assets/Textures/foil/Foil002_1K-PNG_AmbientOcclusion.png");

    */

    // initialize static shader uniforms before rendering
    // --------------------------------------------------
   // glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    //shader.use();
    //shader.setMat4("projection", projection);


    Camera pbrCamera;
    pbrCamera.SetPosition({ 0, 0, 15 });
    
    // load textures
    // -------------
    unsigned int woodTexture = loadTexture("Assets/wood.png");

    // lighting info
    // -------------
    glm::vec3 lightPos(0.0f, 10.0f, 2.0f);

    //Car playerCar("Assets/Models/car/racer_nowheels.obj", glm::vec3{ 0, 0.5f, 0 }, glm::vec3{ 1.0f });
    Car playerCar("Assets/Models/subaru/scene.gltf", glm::vec3{ 0 }, glm::vec3{ 0.0075f });
    //Car playerCar("Assets/Models/dirty/scene.gltf", glm::vec3{ 0 }, glm::vec3{ 1.0f });

    //Car pbrCar("Assets/Models/subaru/scene.gltf", glm::vec3{ 0.0f }, glm::vec3{ 1.0f });
    //Car playerCar("Assets/Models/pbr_gun/scene.gltf", glm::vec3{ 0 }, glm::vec3{ 0.2f });
    FollowCamera followCamera(playerCar);

    std::vector<StaticObject> objects{
       //{ "Assets/Models/moscow/moscow.obj", { 0.0f, 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, glm::vec3{80.0f} }
       //{ "Assets/Models/pirate/port_royale.obj", { 0.0f, -10.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, glm::vec3{40.0f} }
    };

    float near_plane = 1.0f;
    float far_plane = 7.5f;

    glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    

    unsigned int goldAlbedoMap = loadTexture("Assets/Textures/pbr/foil/Foil002_1K-PNG_Color.png");
    unsigned int goldNormalMap = loadTexture("Assets/Textures/pbr/foil/Foil002_1K-PNG_NormalGL.png");
    unsigned int goldMetallicMap = loadTexture("Assets/Textures/pbr/foil/Foil002_1K-PNG_Metalness.png");
    unsigned int goldRoughnessMap = loadTexture("Assets/Textures/pbr/foil/Foil002_1K-PNG_Roughness.png");
    unsigned int goldAOMap = loadTexture("Assets/Textures/pbr/foil/Foil002_1K-PNG_AmbientOcclusion.png");

    

    glm::mat4 model = glm::mat4(1.0f);

    std::vector<Light> lights {
        {{0.0f,  3.0f, 0.0f}, {300.0f, 300.0f, 300.0f}},
        //{{-10.0f,  10.0f, 10.0f}, {300.0f, 300.0f, 300.0f}},
        //{{10.0f,  10.0f, 10.0f}, {300.0f, 300.0f, 300.0f}},
        //{{-10.0f, -10.0f, 10.0f}, {300.0f, 300.0f, 300.0f}},
        //{{10.0f, -10.0f, 10.0f}, {300.0f, 300.0f, 300.0f}}
    };

    bool editorMode = false;

    EditorCamera editorCamera;

    // render loop
    // -----------
    while (!app.WindowShouldClose())
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        app.ProcessInput();

        if (Input::GetKeyDown(GLFW_KEY_TAB))
        {
            editorMode = !editorMode;
            app.SetCursorEnable(editorMode);
        }

        Camera* mainCamera = &followCamera;

        if (editorMode)
            mainCamera = &editorCamera;

        //glm::mat4 projection = pbrCamera.GetProjectionMatrix();
        glm::mat4 projection = mainCamera->GetProjectionMatrix();
        renderer.m_pbrShader.use();
        renderer.m_pbrShader.setMat4("projection", projection);
        renderer.m_backgroundShader.use();
        renderer.m_backgroundShader.setMat4("projection", projection);
        

        if (!editorMode)
        {
            playerCar.Update(deltaTime);
            //playerCar.AudioUpdate(audio, deltaTime, window);
            playerCar.CheckCollisions(objects, deltaTime);
        }

        mainCamera->Update(deltaTime);
        

        renderer.BeginFrame(*mainCamera);

        
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, goldAlbedoMap);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, goldNormalMap);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, goldMetallicMap);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, goldRoughnessMap);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, goldAOMap);
        

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.0, 0.0, 2.0));

        renderer.m_pbrShader.setMat4("model", model);
        renderer.m_pbrShader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
        renderSphere();

        renderScene(renderer, renderer.m_pbrShader);

        playerCar.Render(renderer.m_pbrShader);

        for (auto& obj : objects)
            obj.Render(renderer.m_pbrShader);

        for (unsigned int i = 0; i < lights.size(); ++i)
        {
            glm::vec3 newPos = lights[i].position + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            newPos = lights[i].position;
            renderer.m_pbrShader.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
            renderer.m_pbrShader.setVec3("lightColors[" + std::to_string(i) + "]", lights[i].color);

            model = glm::mat4(1.0f);
            model = glm::translate(model, newPos);
            model = glm::scale(model, glm::vec3(0.5f));
            renderer.m_pbrShader.setMat4("model", model);
            renderer.m_pbrShader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
            renderSphere();
        }


        renderer.RenderSkybox();

        /*
        renderer.RenderDepthMap(lightSpaceMatrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);

        renderScene(renderer, renderer.m_depthShader);
        playerCar.Render(renderer.m_depthShader);

        for (auto& obj : objects)
            obj.Render(renderer.m_depthShader);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        */

        //RenderPbrScene(shader, pbrCamera);
        
        /*
        renderer.RenderLighting(lightPos, lightSpaceMatrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);

        renderScene(renderer, renderer.m_baseShader);
        playerCar.Render(renderer.m_baseShader);

        for (auto& obj : objects)
            obj.Render(renderer.m_baseShader);


        
        debugDepthQuad.use();
        debugDepthQuad.setFloat("near_plane", near_plane);
        debugDepthQuad.setFloat("far_plane", far_plane);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderer.m_depthMap);
        //renderQuad();
        */

        if (editorMode)
        {
            imgui.Begin();
            imgui.Render(pbrCamera, playerCar, renderer);
            imgui.RenderLights(lights);
            imgui.End();
        }


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(app.GetWindow());
        //glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void RenderPbrScene(Shader& shader, Camera& camera)
{
    // lights
// ------
    glm::vec3 lightPositions[] = {
        glm::vec3(0.0f, 0.0f, 10.0f),
    };
    glm::vec3 lightColors[] = {
        glm::vec3(150.0f, 150.0f, 150.0f),
    };
    int nrRows = 7;
    int nrColumns = 7;
    float spacing = 2.5;

    shader.use();
    //glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 view = glm::mat4(1.0f);
    shader.setMat4("view", camera.GetViewMatrix());
    //shader.setVec3("camPos", camera.Position);
    shader.setVec3("camPos", camera.GetPosition());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, albedo);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, metallic);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, roughness);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, ao);

    // render rows*column number of spheres with material properties defined by textures (they all have the same material properties)
    glm::mat4 model = glm::mat4(1.0f);
    for (int row = 0; row < nrRows; ++row)
    {
        for (int col = 0; col < nrColumns; ++col)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(
                (float)(col - (nrColumns / 2)) * spacing,
                (float)(row - (nrRows / 2)) * spacing,
                0.0f
            ));
            shader.setMat4("model", model);
            shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
            renderSphere();
        }
    }

    // render light source (simply re-render sphere at light positions)
    // this looks a bit off as we use the same shader, but it'll make their positions obvious and 
    // keeps the codeprint small.
    for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
    {
        glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
        newPos = lightPositions[i];

        shader.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
        shader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

        model = glm::mat4(1.0f);
        model = glm::translate(model, newPos);
        model = glm::scale(model, glm::vec3(0.5f));
        shader.setMat4("model", model);
        shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
        renderSphere();
    }
}

// renders the 3D scene
// --------------------
void renderScene(Renderer& renderer, const Shader& shader)
{
    // floor
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);
    renderer.DrawPlane();

    // cubes
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    //renderer.DrawCube();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    //renderer.DrawCube();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(0.25));
    shader.setMat4("model", model);
    //renderer.DrawCube();
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    /*
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
        */
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
