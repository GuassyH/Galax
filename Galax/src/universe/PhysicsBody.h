#pragma once

#include <glm/glm.hpp>

struct PhysicsBody
{
	float mass = 1.0f;
	glm::vec3 velocity = glm::vec3(0.0f);
};