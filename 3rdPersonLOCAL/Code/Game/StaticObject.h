#pragma once

#include <learnopengl/model_animation.h>
#include "GameObject.h"

class StaticObject : public GameObject
{
public:
	StaticObject(const std::string& modelPath)
		: m_model(modelPath)
	{
	}

	virtual void Update(float dt) override
	{

	}

	virtual void Render(Renderer& renderer) override
	{
		renderer.DrawModel(m_model, m_position, m_scale, m_rotation);

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

protected:
	Model m_model;
};