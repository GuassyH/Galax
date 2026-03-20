#pragma once

#include <glm/glm.hpp>

struct Vertex {
	glm::vec3 position;
	float pad1 = 0.0f;
	glm::vec3 normal;
	float pad2 = 0.0f;
	glm::vec2 texCoord;
	float pad3[2] = { 0.0f, 0.0f };
};