#pragma once

#include <glm/glm.hpp>

enum class LightType
{
	None,
	Directional,
	Point,
	Spot,
};

struct Light
{
	LightType type = LightType::None;
	glm::vec3 position { 0.0f };
	glm::vec3 color { 1.0f };

	Light() = default;

	Light(const glm::vec3& p, const glm::vec3& c)
		: position(p)
		, color(c)
	{}
};