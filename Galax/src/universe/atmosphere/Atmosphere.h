#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>

#include "Camera.h"
#include "shaders/FragShader.h"
#include "shaders/ComputeShader.h"
#include "rendering/Mesh.h"


namespace Universe {

	class Planet;

	// allign to nearest 16 bytes
	struct AtmosphereConfig {
		float atmosphereHeight = 50.0f; // Default 50.0
		float intensity = 1.0f; // Default 1.0
		float densityFalloff = 5.0f; // Default 5.0

		glm::vec3 wavelengths = glm::vec3(700.0f, 550.0f, 440.0f); // Default 700, 550, 440
		glm::vec3 wavelengthScatter; // SET AUTOMATICALLY

		float scatteringCoefficient = 380.0f; // Default 380
		float scatteringStrength = 0.75f; // Default 0.75
		
		void UpdateWavelengthScatter() {
			float scatterR = glm::pow(scatteringCoefficient / wavelengths.x, 4) * scatteringStrength;
			float scatterG = glm::pow(scatteringCoefficient / wavelengths.y, 4) * scatteringStrength;
			float scatterB = glm::pow(scatteringCoefficient / wavelengths.z, 4) * scatteringStrength;

			wavelengthScatter = glm::vec3(scatterR, scatterG, scatterB);
		}

		GLuint BakedOpticalTexture;
	};


	class AtmosphereRenderer {
	public:
		AtmosphereRenderer();
		~AtmosphereRenderer();

		int numInScatteringPoints = 8;
		int numOpticalDepthPoints = 8;

		void Render(Camera& camera, Universe::Planet* sun, Universe::Planet* planet, AtmosphereConfig& atmos_config, GLuint screenTex, GLuint depthTex);
		void BakeOpticalDepth(AtmosphereConfig& atmosphereConfig, float planetRadius);

	private:
		Mesh quad;

		ComputeShader opticalBakeComp;
		FragShader atmosphereShader;
	};

};