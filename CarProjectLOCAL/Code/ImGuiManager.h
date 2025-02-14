#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "Application.h"
#include "Light.h"
#include "Renderer.h"

#include <vector>

class Camera;
class Car;

class ImGuiManager
{
public:
	ImGuiManager();
	~ImGuiManager();

	void Render(Camera& camera, Renderer& renderer);
	void RenderLights(std::vector<Light>& lights);
	void Begin();
	void End();
};
