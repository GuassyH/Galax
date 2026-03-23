#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>

#include "Camera.h"
#include "shaders/FragShader.h"
#include "Mesh.h"
#include "universe/planetary/Planet.h"

namespace Universe {

	// allign to nearest 16 bytes
	struct AtmosphereConfig {
		float planetRadius = 1.0f; // Default 1.0
		float atmosphereHeight = 50.0f; // Default 50.0
		float intensity = 1.0f; // Default 1.0
		float densityFalloff = 5.0f; // Default 5.0

		glm::vec3 centre = glm::vec3(0.0f);
		float pad1;
		glm::vec3 wavelengths = glm::vec3(700.0f, 550.0f, 440.0f); // Default 700, 550, 440
		float pad2;
		glm::vec3 scatteringCoefficients; // SET AUTOMATICALLY
		float pad3;

		float scatteringCoefficient = 380.0f; // Default 380
		float scatteringStrength = 0.75f; // Default 0.75
		float pad4[2];
	};


	class AtmosphereRenderer {
	public:
		AtmosphereRenderer();
		~AtmosphereRenderer();

		int numInScatteringPoints = 8;
		int numOpticalDepthPoints = 8;

		void UpdateBuffers();
		void Render(Camera& camera, Universe::Planet& sun, GLuint screenTex, GLuint starTex, GLuint depthTex, int w, int h);

		std::vector<AtmosphereConfig> atmosphere_configs;
		
	private:
		Mesh quad;

		int last_width = 0;
		int last_height = 0;

		GLuint atmosphereBuffer;
		FragShader atmosphereShader;
	};

};