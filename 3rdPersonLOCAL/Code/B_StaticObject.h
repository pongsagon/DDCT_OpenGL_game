#include "BanKEngine.h"
#include <learnopengl/model_animation.h>



class B_StaticObject : public BanKBehavior {

public:
	B_StaticObject(const std::string& modelPath)
		: m_model(modelPath)
	{
	}


	void Render(Renderer& renderer)
	{ 
		renderer.DrawModel(m_model, GameObject->Transform.modelMatrix);

		/*
		Shader& shader = renderer.m_basicShader;
		shader.use();
		glm::mat4 model = glm::translate(glm::mat4(1.0f), m_position)
			* glm::mat4(glm::quat(m_rotation))
			* glm::scale(glm::mat4(1.0f), m_scale);
		shader.setMat4("model", model);
		m_model.Draw(shader);
		*/
	}
	Model m_model;
};