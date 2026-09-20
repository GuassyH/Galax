#pragma once

#include <glfw/glfw3.h>
#include "rendering/Mesh.h"
#include "shaders/FragShader.h"

class Camera;

namespace Universe {

	class Planet;

	struct CloudConfig {
		float maxHeight = 0;
		float minHeight = 0;


	};


	class CloudRenderer {
	public:
		CloudRenderer();

		void Render(Camera& camera, Universe::Planet* sun, Universe::Planet* planet, CloudConfig& atmos_config, GLuint bakedOpticalDepth, GLuint screenTexture, GLuint depthTexture);
	
	private:
		FragShader cloudShader;
		Mesh quad;
	};


}