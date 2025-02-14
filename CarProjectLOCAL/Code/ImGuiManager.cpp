#include "ImGuiManager.h"
#include "Car.h"
#include "Camera.h"

ImGuiManager::ImGuiManager()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImFontConfig fontConfig;
	fontConfig.SizePixels = 16.0f;
	io.Fonts->AddFontDefault(&fontConfig);

	ImGui::StyleColorsDark();
	GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow());

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

}

ImGuiManager::~ImGuiManager()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiManager::Begin()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

glm::vec3 InputVec3(const std::string& text, const glm::vec3 & v)
{
	float values[3]{ v.x, v.y, v.z };
	ImGui::InputFloat3(text.c_str(), values);

	return { values[0], values[1], values[2] };
}

void ImGuiManager::Render(Camera& camera, Renderer& renderer)
{
	ImGui::Begin("Main");

	glm::vec3 tempPos = InputVec3("Camera Pos", camera.GetPosition());
	camera.SetPosition(tempPos);

	if (ImGui::Button("Recompile shaders"))
	{
		renderer.RecompileShaders();
	}

	ImGui::End();
}

void ImGuiManager::RenderLights(std::vector<Light>& lights)
{
	ImGui::Begin("Lights");

	if (ImGui::Button("Add"))
	{
		lights.push_back(Light{});
	}

	int toDeleteLight = -1;

	for (int i = 0; i < lights.size(); i++)
	{
		ImGui::Separator();
		ImGui::Text(("Light " + std::to_string(i + 1)).c_str());

		ImGui::PushID(i);
		glm::vec3 tempPos = InputVec3("Pos", lights[i].position);

		if (ImGui::Button("Delete"))
		{
			toDeleteLight = i;
		}

		ImGui::PopID();

		lights[i].position = tempPos;
	}

	if (toDeleteLight >= 0)
	{
		lights.erase(lights.begin() + toDeleteLight);
	}

	ImGui::End();
	//for ()
}

void ImGuiManager::End()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}