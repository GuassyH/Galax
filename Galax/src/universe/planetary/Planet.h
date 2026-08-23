#pragma once

#include <iostream>
#include <vector>

#include "universe/PhysicsBody.h"
#include "CubeSphere.h"
#include "Transform.h"
#include "shaders/ComputeShader.h"
#include "shaders/PlanetShader.h"
#include "TerrainGenerator.h"

#include "universe/atmosphere/Atmosphere.h"
#include "universe/ocean/Ocean.h"
#include "rendering/Renderer.h"

namespace Universe {


	class Planet {
	public:
		const char* name = "new_planet";

		/// Core
		Planet() = default;
		~Planet() = default;
		void Generate(Renderer& renderer);
	
		void RemoveNodesFromVSSBO(Renderer& renderer, CubeSphere::Chunk* chunk);
		void Update(Renderer& renderer, Camera& camera, bool isSimulating);
		void UpdateLOD(Renderer& renderer, CubeSphere::Chunk* chunk, glm::vec3& observer_pos);
		void UpdateAllLODs(Renderer& renderer, glm::vec3 observer_pos);
		void Render(Renderer& renderer, Camera& camera, Planet* sun);
	
		void Delete();

		// Mesh stuff
		float radius = 10.0f;
		int resolution = 10;
		std::vector<float> LODradii = { 4.8f, 3.4f, 2.2f, 1.6f, 1.2f }; // How many radii until the next LOD, at 4.8*r we subdivide once, at 3.4*r we do again

		TerrainGenerator terrainGenerator;
		std::vector<CubeSphere::Face> faces;
		std::shared_ptr<Transform> transform;

		// Material stuff
		PlanetShader shader;
		float ambient = 0.0f;

		// Body
		float mpr = 0.0f; // minutes
		glm::vec3 rotation_axis = glm::vec3(0.0f, 1.0f, 0.0f);
		PhysicsBody physicsBody;

		bool hasAtmosphere = false;
		AtmosphereConfig atmosphere_config;

		bool hasOcean = false;
		OceanConfig ocean_config;
	};


}