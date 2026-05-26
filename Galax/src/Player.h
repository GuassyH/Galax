#pragma once

#include "Camera.h"
#include "Transform.h"
#include "universe/planetary/Planet.h"

class Player {
public:
	Player();
	~Player();

	float speed = 8.0f;
	float sensitivity = 0.002f;

	void Move();
	void Look();

	void AllignToPlanet(Universe::Planet* planet, float _0_1_val);


	std::shared_ptr<Transform> transform;
	Camera camera;

private:
	Universe::Planet* parent_planet = nullptr;
};