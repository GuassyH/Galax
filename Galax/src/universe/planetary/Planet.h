#pragma once

#include <iostream>
#include <vector>

#include "universe/PhysicsBody.h"
#include "CubeSphere.h"
#include "Transform.h"
#include "shaders/ComputeShader.h"
#include "shaders/FragShader.h"
#include "TerrainGenerator.h"

#include "universe/atmosphere/Atmosphere.h"
#include "universe/ocean/Ocean.h"

namespace Universe {
	class Planet {
	public:
		struct AngleColor {
			glm::vec4 color;
			float angle;
			float pad[3];
		};

		/// Core
		Planet() = default;
		~Planet() = default;
		void Generate();
	
		void Update(Camera& camera);
		void UpdateLOD(CubeSphere::Chunk* chunk, glm::vec3& observer_pos);
		void UpdateAllLODs(glm::vec3 observer_pos);
		void Render(Camera& camera, Planet* sun);
	
		void Delete();

		// Mesh stuff
		float radius = 10.0f;
		int resolution = 10;
		std::vector<float> LODradii = { 4.8f, 3.4f, 2.2f, 1.6f, 1.2f }; // How many radii until the next LOD, at 4.8*r we subdivide once, at 3.4*r we do again

		TerrainGenerator terrainGenerator;
		std::vector<CubeSphere::Face> faces;
		std::shared_ptr<Transform> transform;

		// Material stuff
		FragShader planetShader;
		std::vector<AngleColor> angles_to_colors = { };
		float ambient = 0.0f;

		// Body
		float mass = 1.0f;
		float mpr = 0.0f; // minutes
		glm::vec3 rotation_axis = glm::vec3(0.0f, 1.0f, 0.0f);
		PhysicsBody physicsBody;

		bool hasAtmosphere = false;
		AtmosphereConfig atmosphere_config;

		bool hasOcean = false;
		OceanConfig ocean_config;
	private:
		ComputeShader terrain_compute;
		ComputeShader normals_compute;
	};
}