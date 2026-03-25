#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>

#include "Camera.h"
#include "shaders/FragShader.h"
#include "Mesh.h"
#include "universe/planetary/Planet.h"

namespace Universe {

	struct OceanConfig {
		glm::vec3 centre;
	};

	class OceanRenderer {
	public:
		OceanRenderer();
		~OceanRenderer();

		std::vector<OceanConfig> ocean_configs;

	private:
		Mesh quad;

		int last_width = 0;
		int last_height = 0;

		GLuint oceanBuffer;
		FragShader oceanShader;
	};

}