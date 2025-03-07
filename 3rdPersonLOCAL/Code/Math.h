#pragma once

#include <iomanip>

class Math
{
public:
	static float Clamp(float value, float minValue, float maxValue)
	{
		return std::max(minValue, std::min(value, maxValue));
	}

	static glm::vec3 Lerp(const glm::vec3& a, const glm::vec3& b, float t)
	{
		return (1.0f - t) * a + t * b;
	}

	static float Lerp(float a, float b, float t)
	{
		return a + t * (b - a);
	}
};