#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <learnopengl/model_animation.h>

class ResourceManager
{
public:
	inline static ResourceManager* s_instance = nullptr;

	static ResourceManager* Get()
	{
		return s_instance;
	}

	ResourceManager()
	{
		s_instance = this;
	}

	void LoadModel(const std::string& name, const std::string& path)
	{
		if (m_modelMap.find(name) != m_modelMap.end())
			m_modelMap[name].release();

		m_modelMap[name] = std::make_unique<Model>(path);
	}

	Model* GetModel(const std::string& name)
	{
		if (m_modelMap.find(name) == m_modelMap.end())
			return nullptr;

		return m_modelMap[name].get();
	}

	std::unordered_map<std::string, std::unique_ptr<Model>> m_modelMap;
};