#pragma once

#include "CubeSphere.h"
#include <iostream>
#include "Transform.h"

#include <vector>

namespace Universe {
	class Planet {
	public:
		/// Core
		Planet();
		~Planet();
	
		void Update(Camera& camera);
		void UpdateLOD(CubeSphere::Chunk* chunk, glm::vec3& observer_pos);
		void UpdateAllLODs(glm::vec3 observer_pos);
		void Render(Camera& camera, Shader& shader);
	
		void Delete();

		float radius = 10.0f;

		// How many radii until the next LOD, at 4*r we subdivide once, at 3*r we do again
		// std::vector<float> LODradii = { 4.0f, 3.0f, 2.0f, 1.2f };
		std::vector<float> LODradii = { 2.0f, 1.5f, 1.25f, 1.1f };

		std::vector<CubeSphere::Face> faces;
		std::unique_ptr<Transform> transform;
	};
}