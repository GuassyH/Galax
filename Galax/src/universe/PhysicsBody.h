#pragma once

#include <glm/glm.hpp>

namespace Universe {
	class Planet;
}

struct PhysicsBody
{
	float mass = 1.0f;
	glm::vec3 velocity = glm::vec3(0.0f);

	bool debug_path = false;
	Universe::Planet* debug_centre = nullptr;
	int num_points = 1000;
	float time_step = 8.0f;
};