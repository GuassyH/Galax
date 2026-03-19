#pragma once

#include <glm/glm.hpp>

struct Vertex {
	glm::vec3 position;
	float pad1;
	glm::vec3 normal;
	float pad2;
	glm::vec2 texCoord;
	float pad3[2];
};