#pragma once

#include "Camera.h"
#include "Transform.h"
#include "universe/planetary/Planet.h"

class Player {
public:
	Player();
	~Player();

	float speed = 10.0f;
	float sensitivity = 0.002f;


	void Move(GLFWwindow* window);
	void Look(GLFWwindow* window);

	void SetParentPlanet(Universe::Planet* planet);


	std::shared_ptr<Transform> transform;
	Camera camera;

private:
	Universe::Planet* parent_planet = nullptr;
};