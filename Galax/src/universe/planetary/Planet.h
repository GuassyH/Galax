#pragma once

#include <iostream>
#include <vector>


#include "CubeSphere.h"
#include "Transform.h"
#include "shaders/ComputeShader.h"
#include "TerrainGenerator.h"

namespace Universe {
	class Planet {
	public:
		/// Core
		Planet() = default;
		~Planet() = default;
		void Generate();
	
		void Update(Camera& camera);
		void UpdateLOD(CubeSphere::Chunk* chunk, glm::vec3& observer_pos);
		void UpdateAllLODs(glm::vec3 observer_pos);
		void Render(Camera& camera, Shader& shader);
	
		TerrainGenerator terrainGenerator;

		void Delete();

		float radius = 10.0f;
		int resolution = 10;

		// How many radii until the next LOD, at 4*r we subdivide once, at 3*r we do again
		// std::vector<float> LODradii = { 2.0f, 1.5f, 1.25f, 1.1f };
		std::vector<float> LODradii = { 4.8f, 3.4f, 2.2f, 1.6f, 1.2f };

		std::vector<CubeSphere::Face> faces;
		std::shared_ptr<Transform> transform;

	private:
		ComputeShader terrain_compute;
		ComputeShader normals_compute;
	};
}