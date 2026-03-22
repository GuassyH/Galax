#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>

#include "Camera.h"
#include "shaders/FragShader.h"
#include "Mesh.h"

namespace Universe {

	// allign to nearest 16 bytes
	struct AtmosphereConfig {
		float planetRadius = 1.0f;
		float atmosphereHeight = 50.0f;
		float intensity = 1.0f;
		float densityFalloff = 5.0f;

		glm::vec3 centre = glm::vec3(0.0f);
		float pad1;
		glm::vec3 wavelengths = glm::vec3(700.0f, 530.0f, 440.0f);
		float pad2;
		glm::vec3 scatteringCoefficients; // SET AUTOMATICALLY
		float pad3;

		float scatteringCoefficient = 380.0f;
		float scatteringStrength = 0.75f;
		float pad4[2];
	};


	class AtmosphereRenderer {
	public:
		AtmosphereRenderer();
		~AtmosphereRenderer();

		int numInScatteringPoints = 8;
		int numOpticalDepthPoints = 9;

		void UpdateBuffers();
		void Render(Camera& camera, int w, int h);

		std::vector<AtmosphereConfig> atmosphere_configs;
		
		GLuint framebuffer = 0;
	private:
		GLuint screenTexture = 0;
		GLuint depthTexture = 0;

		Mesh quad;

		int last_width = 0;
		int last_height = 0;

		GLuint atmosphereBuffer;
		FragShader atmosphereShader;
	};

};