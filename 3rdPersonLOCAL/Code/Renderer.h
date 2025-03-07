#pragma once


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <learnopengl/shader_m.h>
#include "Camera.h"
#include "Light.h"
#include "learnopengl/model_animation.h"

#include <vector>

enum class DrawCommandType
{
	None,
	Quad,
	Cube,
	Model,
	Animation
};

struct DrawCommand
{
	DrawCommandType type = DrawCommandType::None;
	glm::mat4 model;
	Model* mesh = nullptr;
	std::vector<glm::mat4> boneMatrices;
	glm::vec3 color;
};


class Renderer
{
public:
	Renderer();
	~Renderer();

	void SetupPBR(const std::string& cubeMapPath);

	void Clear();
	void BeginFrame(Camera& camera);

	void RenderScene(Camera& camera, const std::vector<Light>& lights);

	void DrawPlane();
	void DrawCube();

	void DrawModel(Model& mesh, const glm::vec3& position, const glm::vec3& scale, const glm::vec3& rotation);
	void DrawModel(Model& mesh, const glm::mat4& modelMatrix);
	void DrawAnimation(Model& mesh, const glm::vec3& position, const glm::vec3& scale, const glm::vec3& rotation, const std::vector<glm::mat4>& boneMatrices);
	void DrawAnimation(Model& mesh, const glm::mat4& modelMatrix, const std::vector<glm::mat4>& boneMatrices);


	void RenderDepthMap(const glm::mat4& lightSpaceMatrix);
	void RenderLighting(const glm::vec3& lightPosition, const glm::mat4& lightSpaceMatrix);
	void RenderSkybox();

	void RecompileShaders();

	Shader m_baseShader;
	Shader m_depthShader;
	Shader m_pbrShader;
	Shader m_equirectangularToCubemapShader;
	Shader m_irradianceShader;
	Shader m_prefilterShader;
	Shader m_brdfShader;
	Shader m_backgroundShader;
	Shader m_animShader;
	Shader m_basicShader;
	Shader m_lightingShader;

	unsigned int m_depthMapFBO;
	unsigned int m_depthMap;

	unsigned int m_captureFBO;
	unsigned int m_captureRBO;

	unsigned int m_planeVBO;
	unsigned int m_planeVAO;

	unsigned int m_cubeVAO = 0;
	unsigned int m_cubeVBO = 0;

	unsigned int m_irradianceMap;
	unsigned int m_prefilterMap;
	unsigned int m_brdfLUTTexture;
	unsigned int m_envCubemap;



private:
	void SetupDepthMap();
	void SetupPlane();
	void SetupCube();

	Camera* m_camera = nullptr;
	static Camera s_defaultCamera;

	std::vector<DrawCommand> m_drawCommands;
	std::vector<Light> m_lights;
};