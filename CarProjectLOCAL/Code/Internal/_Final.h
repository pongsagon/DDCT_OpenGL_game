#include "_Def4.h"
#include "_Final/Raycast.h"


class Destroyer : public BanKBehavior {
    public:
        float Lifespan = 5;
        void Update() {
            if (Lifespan <= 0) {
                GameObject->Destroy= true;
            }
            else
            {
                Lifespan -= Time.Deltatime;
            }
        }
}; 
Destroyer* Edit_Destroyer;

/*

namespace BanKEngine {


    			///////////////////////////////////////////
			   /////////////							 /
			  /////////////          IMGUI  			/
			 /////////////							   /
			///////////////////////////////////////////
            namespace EngineUI {
                                        bool Hide = false;
                                        struct BanK_StyleIMGUI {
                                            const ImVec4 Idle = ImVec4(0.14, 0.21, 0.21, 0.95);
                                            const ImVec4 Hover = ImVec4(0.21, 0.32, 0.32, 0.95);
                                            const ImVec4 Active = ImVec4(0.4, 0.6, 0.6, 0.95);
                                            const ImVec4 Background = ImVec4(0.06, 0.09, 0.09, 0.95);
                                            const ImVec4 MenuBar = ImVec4(0.05, 0.07, 0.07, 0.1);
                                        }BanK_Style;

                void Initiate() {

                    IMGUI_CHECKVERSION();
                    ImGui::CreateContext();
                    ImGuiIO& io = ImGui::GetIO();
                    (void)io;

                    ImGui::StyleColorsDark();
                    ImGui_ImplGlfw_InitForOpenGL(BanKEngine::System::Window, true);
                    ImGui_ImplOpenGL3_Init("#version 330 core");

                    io.FontGlobalScale = 1.55;
                    ImGuiStyle& Style = ImGui::GetStyle();
                    Style.ScaleAllSizes(2); 

                    //MENU bar
                    Style.Colors[ImGuiCol_MenuBarBg] = BanK_Style.MenuBar;
                    Style.Colors[ImGuiCol_HeaderHovered] = BanK_Style.Hover;

                    //Window
                    Style.Colors[ImGuiCol_TitleBgActive] = BanK_Style.Idle;
                    Style.Colors[ImGuiCol_WindowBg] = BanK_Style.Background;
                    Style.Colors[ImGuiCol_Border] = BanK_Style.Idle;

                    //Button
                    Style.Colors[ImGuiCol_Button] = BanK_Style.Idle;
                    Style.Colors[ImGuiCol_ButtonHovered] = BanK_Style.Hover;
                    Style.Colors[ImGuiCol_ButtonActive] = BanK_Style.Active;

                    //Slider
                    Style.Colors[ImGuiCol_FrameBg] = BanK_Style.Idle;
                    Style.Colors[ImGuiCol_FrameBgHovered] = BanK_Style.Hover;
                    Style.Colors[ImGuiCol_FrameBgActive] = BanK_Style.Active;
                    Style.Colors[ImGuiCol_SliderGrab] = BanK_Style.Background;
                    Style.Colors[ImGuiCol_SliderGrabActive] = BanK_Style.Active;

                    //ResizeGrip
                    Style.Colors[ImGuiCol_ResizeGrip] = BanK_Style.Idle;
                    Style.Colors[ImGuiCol_ResizeGripHovered] = BanK_Style.Hover;
                    Style.Colors[ImGuiCol_ResizeGripActive] = BanK_Style.Active;
                }

                void Render() {      
                    if (Input::Key::C.OnPressed) { Hide = !Hide; }
                    if(Hide){return;}

                    ImGui_ImplOpenGL3_NewFrame();
                    ImGui_ImplGlfw_NewFrame();
                    ImGui::NewFrame();
                    ///////////////////////////////////////////

                            ImGui::Begin("Performance Monitor", nullptr, ImGuiWindowFlags_NoTitleBar);
                            ImGui::Text("FPS: %d/%d", Time.FPS_AVG, Time.FPS);
                            ImGui::Text("GameOBJ: %d/%d", sGameObjs.size(), sGameObjs.size());
                            ImGui::Text("\nComponent: %d", sBanKBehavior.size());
                            ImGui::Text(" | Render: %d", sRenderers.size());
                            ImGui::Text(" | Transform: %d", sTransforms.size());
                            ImGui::End();

                    //////////////////////////////////////////
                    ImGui::Render();
                    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                }
            }
}

*/