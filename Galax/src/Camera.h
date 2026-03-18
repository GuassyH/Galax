#pragma once

#include <glm/glm.hpp>
#include "Transform.h"

#include <glfw/glfw3.h>
#include <iostream>

class Camera {
public:
	Camera();
	~Camera();

	void UpdateMatrix(unsigned int windowWidth, unsigned int windowHeight);
	void Move(GLFWwindow* window);
	void Look(GLFWwindow* window);

	float fovDeg = 60.0f;
	float nearPlane = 0.1f;
	float farPlane = 1000.0f;

	float speed = 0.1f;
	float sensitivity = 0.002f;

	glm::mat4& GetView() { return view; }
	glm::mat4& GetProj() { return proj; }

	std::shared_ptr<Transform> transform; // many objects will reference this
private:
	glm::mat4 view;
	glm::mat4 proj;
};