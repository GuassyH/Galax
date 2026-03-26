#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>

#include "Camera.h"
#include "shaders/FragShader.h"
#include "Mesh.h"

namespace Universe {

	class Planet;

	struct OceanConfig {
		float radius = 1.0f;
		float densityFalloff = 2.0f;
	};

	class OceanRenderer {
	public:
		OceanRenderer();
		~OceanRenderer();

		void Render(Camera& camera, Universe::Planet* sun, Transform* planet, OceanConfig& ocean_config, GLuint depthTexCopy, int w, int h);

	private:
		Mesh quad;

		FragShader oceanShader;
	};

}