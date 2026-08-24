#pragma once

#include <glm/glm.hpp>

struct Vertex {
	glm::vec3 position;
	float pad1 = 0.0f;
	glm::vec3 normal;
	float pad2 = 0.0f;
	glm::vec4 params; // Tex X, Tex Y, noise, ???
};