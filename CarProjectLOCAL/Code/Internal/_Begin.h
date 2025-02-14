#pragma once


#include <sstream>//String Features
#include <cstdlib>
#include <iostream>
#include <random>
#include <functional>//Fuction Syntaxes
#include <chrono>//Time & Clocks & Random?

#include <optional>
#include <limits>
#include <iomanip>

#include <map>
#include <queue>

//Systems & BasicRenders
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//Shader
//#include "learnopengl/shader_s.h"//1_
//#include "learnopengl/shader_m.h"//3_
#include <learnopengl/shader.h>//Shader Tools - ANY?
 
//Models
//#include <learnopengl/model.h>

//Texture
#include <stb_image.h>

//Camera
//#include <learnopengl/camera.h> 
#include "../Camera.h"


//Math & Tools
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


using namespace std;



//        glEndQuery(GL_TIME_ELAPSED); timeQuery????????
struct Time {
	float Deltatime = 0;
	float TrueDeltatime = 0;
	float CurrentTime = 0;
	double CurrentFrameTime = 0;
	int Frame = 0;
	float Fps = 0;
	float Scale = 1;

	struct Advanced {
		double lastFrameTime = 0;
		double lastSecondTime = 0;
		int framesInLastSecond = 0;
	} Advanced;

	void Calculate() {
		//float currentFrame = static_cast<float>(glfwGetTime());
		//deltaTime = currentFrame - lastFrame;
		//lastFrame = currentFrame;

		CurrentFrameTime = glfwGetTime();
		TrueDeltatime = static_cast<float>(CurrentFrameTime - Advanced.lastFrameTime); // Time in seconds
		Deltatime = TrueDeltatime * Scale;
		Advanced.lastFrameTime = CurrentFrameTime;
		CurrentTime += Deltatime;
		Frame++;

		Fps = 1.0f / Deltatime;
		//std::cout << "FPS: " << Fps << std::endl;
	}
}Time;



float B_frand(float min, float max) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(min, max);
	return dis(gen);
}
int B_irand(int min, int max) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(min, max);
	return dis(gen);
}

float B_distance1D(float A, float B) {
	return abs(A - B);
}
float B_distance2D(glm::vec2& p1, const glm::vec2& p2) {
	float dx = p2.x - p1.x;
	float dy = p2.y - p1.y;
	return std::sqrt(dx * dx + dy * dy);
}
float B_distance2D(float x1, float y1, float x2, float y2) {
	float dx = x2 - x1;
	float dy = y2 - y1;
	return std::sqrt(dx * dx + dy * dy);
}

float B_normalize(float value, float min_val, float max_val) {
	return (value - min_val) / (max_val - min_val);
}
float B_normalize_reversed(float value, float min_val, float max_val) {
	return (value - max_val) / (min_val - max_val);
}
float B_clamp(float value, float minValue, float maxValue) {
	//max(A,B) returns A if A is the max value
	return std::max(minValue, std::min(value, maxValue));
}
float B_clampLoop(float value, float minValue, float maxValue) {
	if (value > maxValue) {
		return minValue;
	}
	else if (value < minValue) {
		return maxValue;
	}
	return value;
}
float B_clampSkin(float value, float minValue, float maxValue) {
	float middle = (minValue + maxValue) / 2.0f;
	if (value < middle) { return minValue; }
	return maxValue;
}

float B_lerp(float start, float end, float t) {
	//t = B_clamp(t, 0, 1);
	return start + t * (end - start);
}
glm::vec3 B_lerpVec3(glm::vec3 A, glm::vec3 B, float t) {
	return (1.0f - t) * A + t * B;
}

float B_SnapToGrid(float& V, float gridSize) {
	return gridSize * std::roundf(V / gridSize);
}

