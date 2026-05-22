#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>

#include "Camera.h"
#include "shaders/FragShader.h"
#include "rendering/Mesh.h"
#include "rendering/Texture.h"

namespace Universe {

	class Planet;

	struct OceanConfig {
		float radius = 1.0f;
		float densityFalloff = 2.0f;
		glm::vec4 oceanColor = glm::vec4(0.35, 0.55, 0.85, 1.0);
		glm::vec4 fresnelColor = glm::vec4(0.35, 0.55, 0.85, 1.0);
		std::shared_ptr<Texture> normalTexture;
		float normalRepeat = 1.0f;
		float normalStrength = 1.0f;
		float triplanarBlend = 16;
		int normalFactor = 64;
	};

	class OceanRenderer {
	public:
		OceanRenderer();
		~OceanRenderer();

		void Render(Camera& camera, Universe::Planet* sun, Transform* planet, OceanConfig& ocean_config, GLuint colorTex, GLuint depthTex);

	private:
		Mesh quad;

		FragShader oceanShader;
	};

}