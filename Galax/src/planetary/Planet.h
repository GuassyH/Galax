#pragma once

#include "CubeSphere.h"
#include <iostream>
#include "Transform.h"

#include <vector>

#include "shaders/ComputeShader.h"

namespace Universe {
	class Planet {
	public:
		/// Core
		Planet();
		~Planet();
		void Generate();
	
		void Update(Camera& camera);
		void UpdateLOD(CubeSphere::Chunk* chunk, glm::vec3& observer_pos);
		void UpdateAllLODs(glm::vec3 observer_pos);
		void Render(Camera& camera, Shader& shader);
	
		void ApplyTerrain(CubeSphere::Chunk* chunk);

		void Delete();

		float radius = 10.0f;
		int resolution = 10;

		// How many radii until the next LOD, at 4*r we subdivide once, at 3*r we do again
		// std::vector<float> LODradii = { 2.0f, 1.5f, 1.25f, 1.1f };
		std::vector<float> LODradii = { 4.8f, 3.4f, 2.2f, 1.6f, 1.2f };

		std::vector<CubeSphere::Face> faces;
		std::unique_ptr<Transform> transform;

	private:
		ComputeShader compute;
	};
}