#pragma once

#include <vector>
#include <glad/glad.h>

#include "rendering/Vertex.h"
#include "Mesh.h"

class CubeSphere {
public:
	static struct Face {
		glm::quat rotation;
		Mesh mesh;
		int resolution = 20;
	};

	static std::vector<Face> ConstructFaces(float radius, Transform* base_transform = nullptr);
};