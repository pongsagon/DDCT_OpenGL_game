#define _CRTDBG_MAP_ALLOC

#include <crtdbg.h>

#include <chrono>
#include "Application.h"
#include "Renderer.h"
#include "FontSystem.h"
#include "ResourceManager.h"
#include "Physics.h"

#include "BanKEngine.h"

#include "B_Camera.h"
#include "B_StaticObject.h"
#include "B_Player.h"
#include "B_Enemy.h"

#include "Input.h"

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
    ImGuiManager imguiManager;
    auto lastTime = std::chrono::high_resolution_clock::now();
	bool playerCamera = false;
    std::vector<Light> lights;




    Steve::Load();
    Doozy::Load();

    //Camera
    GameObj* CameraOBJ = GameObj::Create();
        CameraOBJ->Transform.wPosition = glm::vec3(0, 0, -3); 
        B_Camera* Camera_Bhav = CameraOBJ->AddComponent(new B_Camera);

    ////Scene
    GameObj* SceneOBJ = GameObj::Create();
        B_StaticObject* SceneModel = SceneOBJ->AddComponent(new B_StaticObject("Assets/Models/UE3rd Person/3rdPerson.obj"));
        PLR_Raycast_Init(SceneOBJ, SceneModel->m_model);
        SceneOBJ->Transform.wPosition = glm::vec3{ -5.0f, -1.0f, -5.0f };
        SceneOBJ->Transform.wRotation = glm::vec3(0, 0, 0);
        SceneOBJ->Transform.wScale = glm::vec3(1, 1, 1);

    //Player
    GameObj* PlayerOBJ = GameObj::Create();
        PlayerOBJ->Transform.wPosition = glm::vec3(0, 0, 0);
        Player* Player_Bhav = PlayerOBJ->AddComponent(new Player);
        PLR_Raycast_Init(SceneOBJ, SceneModel->m_model);

        TargetPLR = Player_Bhav;

    GameObj* GUN = GameObj::Create();
        GUN->Transform.wPosition = glm::vec3(0,0,4);
        GUN->AddComponent(new Gun);

    GameObj* EnemyOBJ = GameObj::Create();
        EnemyOBJ->Transform.wPosition = glm::vec3(1, 0, 3);
        EnemyOBJ->AddComponent(new Enemy);


    BanKEngine::Init();
    BanKEngine::All_Start();




    while (!app.WindowShouldClose())
    {
        BanKEngine::All_Update();

        app.ProcessInput();

        if (Input::GetKeyDown(GLFW_KEY_TAB))
        {
            playerCamera = !playerCamera;
            app.SetCursorEnable(!playerCamera);
        }
        //////////////////////////////////////////////////////////////////////




        fontSystem.RenderText("I am a hero", { 100, 100 }, 24, glm::vec4(1.0f));

        if (sGetComponent_OfClass(Player_Bhav)) {
            float LerpSpeed = 16 * Time.Deltatime;
            CameraOBJ->Transform.wPosition = B_lerpVec3(CameraOBJ->Transform.wPosition, Player_Bhav->CamSocket->Transform.getWorldPosition(), LerpSpeed);
            Camera_Bhav->m_lookAt = B_lerpVec3(Camera_Bhav->m_lookAt, Player_Bhav->CamLookat->Transform.getWorldPosition(), LerpSpeed);

            PLR_Raycast_Update(Player_Bhav->GameObject, Player_Bhav);

        }
        else
        {
            PlayerOBJ = GameObj::Create();
            PlayerOBJ->Transform.wPosition = glm::vec3(0, 0, 0);
            Player_Bhav = PlayerOBJ->AddComponent(new Player);

            TargetPLR = Player_Bhav;
        }

        SpawnTimer_Gun += Time.Deltatime;
        if (SpawnTimer_Gun > SpawnTimer_Gun_MAX && GunCount < 5) {
            SpawnTimer_Gun = 0;
            GameObj* GUN = GameObj::Create();
            GUN->Transform.wPosition = glm::vec3(B_frand(-10, 10), 0, B_frand(-10, 10));
            GUN->AddComponent(new Gun);
        }

        //EnemyMax += Time.Deltatime * 0.025;
        //if (EnemyCount < EnemyMax) {
        //    GameObj* Enemy01 = GameObj::Create();
        //    Enemy01->Transform.wPosition = glm::vec3(B_frand(-10, 10), 0, B_frand(-10, 10));
        //    Enemy01->AddComponent(new Enemy);
        //}



        //////////////////////////////////////////////////////////////////////
        renderer.Clear();

        for (GameObj* pInst : sGameObjs) {
            pInst->Render(renderer);
        }


        Camera mainCamera;
        mainCamera.m_position = CameraOBJ->Transform.wPosition;
        mainCamera.m_lookAt = Camera_Bhav->m_lookAt;
        mainCamera.m_fov = Camera_Bhav->m_fov;
        renderer.RenderScene(mainCamera, lights);

        imguiManager.Begin();
        imguiManager.RenderLights(renderer, lights);
        imguiManager.End();
        app.SwapBuffers();
    }


}