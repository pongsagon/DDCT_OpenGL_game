#define _CRTDBG_MAP_ALLOC

#include <crtdbg.h>

#include <chrono>
#include "Application.h"
#include "Renderer.h"
#include "FontSystem.h"
#include "ResourceManager.h"
#include "Physics.h"

#include "Game/Player.h"
#include "Game/Enemy.h"
#include "Game/StaticObject.h"
#include "Game/ThirdPersonCamera.h"
#include "ImGuiManager.h"

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    Application app;
    Renderer renderer;
    glfwSwapInterval(0);

	ResourceManager resourceManager;
    Physics physics;

    OpenGLFontSystem fontSystem;

    Camera sceneCamera;
    sceneCamera.SetPosition({ 0, 4, -4 });
    sceneCamera.SetLookAt({ 0, 0, 0 });

    std::vector<GameObject*> gameObjects;

    //resourceManager.LoadModel("gun", "Assets/Models/AK47/OBJ/ak7finished.obj");
    //resourceManager.LoadModel("bullet", "Assets/Models/Bullets/Bullets.obj");

    Player* player = new Player();
    StaticObject* level = new StaticObject("Assets/Models/UE3rd Person/3rdPerson.obj");
    level->m_position = glm::vec3{ -5.0f, -1.0f, -5.0f };

    gameObjects.push_back(player);
    gameObjects.push_back(new Enemy());
    //gameObjects.push_back(new Gun());
    gameObjects.push_back(level);

	ThirdPersonCamera thirdPersonCamera(*player);
    player->m_thirdPersonCamera = &thirdPersonCamera;

    ImGuiManager imguiManager;

    auto lastTime = std::chrono::high_resolution_clock::now();

	bool playerCamera = false;


    std::vector<Light> lights;

    while (!app.WindowShouldClose())
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltatime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        app.ProcessInput();

        if (Input::GetKeyDown(GLFW_KEY_TAB))
        {
            playerCamera = !playerCamera;
            app.SetCursorEnable(!playerCamera);
        }

		// Update
		for (auto go : gameObjects)
			go->Update(deltatime);

        // Physics
		physics.Update(deltatime);

		// Camera
		thirdPersonCamera.Update(deltatime);


		// Rendering
        Camera& mainCamera = playerCamera ? thirdPersonCamera : sceneCamera;

        renderer.Clear();

        for (auto go : gameObjects)
            go->Render(renderer);


        renderer.RenderScene(mainCamera, lights);

        /*
        renderer.m_animShader.use();
        renderer.m_animShader.setMat4("projection", mainCamera.GetProjectionMatrix());
        renderer.m_animShader.setMat4("view", mainCamera.GetViewMatrix());

        renderer.m_basicShader.use();
        renderer.m_basicShader.setMat4("projection", mainCamera.GetProjectionMatrix());
        renderer.m_basicShader.setMat4("view", mainCamera.GetViewMatrix());

		for (auto go : gameObjects)
			go->Render(renderer);

        fontSystem.RenderText("I am a hero", { 100, 100 }, 24, glm::vec4(1.0f));

        */

        imguiManager.Begin();
        imguiManager.RenderLights(renderer, lights);
        imguiManager.End();

        app.SwapBuffers();
    }

    for (auto go : gameObjects)
        delete go;
}