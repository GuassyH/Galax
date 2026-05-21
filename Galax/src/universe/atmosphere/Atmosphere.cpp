#include "Atmosphere.h"
#include "universe/planetary/Planet.h"
#include "core/Log.h"

#include "core/Input.h"

namespace Universe {

	static std::vector<Vertex> quad_verts = {
	{ {-1.0f, -1.0f, 0.0f }, {}, {}, {}, {}, { 0.0f, 0.0f }, {} },
	{ {1.0f, -1.0f, 0.0f }, {}, {}, {}, {}, { 1.0f, 0.0f }, {} },
	{ {-1.0f, 1.0f, 0.0f }, {}, {}, {}, {}, { 0.0f, 1.0f }, {} },
	{ {1.0f, 1.0f, 0.0f }, {}, {}, {}, {}, { 1.0f, 1.0f }, {} },
	};

	static std::vector<GLuint> quad_inds = {
		0, 1, 2,
		2, 1, 3
	};




	AtmosphereRenderer::AtmosphereRenderer() {
		atmosphereShader.Compile("assets/shaders/universe/atmosphere.frag", "assets/shaders/universe/atmosphere.vert");

		quad = Mesh(quad_verts, quad_inds);
	}

	AtmosphereRenderer::~AtmosphereRenderer() {
		atmosphereShader.Delete();

		quad.Delete();
	}
	

	void AtmosphereRenderer::Render(Camera& camera, Universe::Planet* sun, Transform* planet, AtmosphereConfig& atmos_config, GLuint screenTex, GLuint depthTex, GLuint starTex) {
		glm::vec2 window_size = Galax::InputManager::Get().windowSize;

		atmosphereShader.Use();
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, screenTex);
		atmosphereShader.SetInt("screenTexture", 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthTex);
		atmosphereShader.SetInt("depthTexture", 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, starTex);
		atmosphereShader.SetInt("starTexture", 2);


		atmosphereShader.SetFloat("planetRadius", atmos_config.planetRadius);
		atmosphereShader.SetFloat("atmosphereHeight", atmos_config.atmosphereHeight);
		atmosphereShader.SetFloat("intensity", atmos_config.intensity);
		atmosphereShader.SetFloat("densityFalloff", atmos_config.densityFalloff);
		atmosphereShader.SetVec3("centre", planet->world_position);
		atmosphereShader.SetVec3("wavelengths", atmos_config.wavelengths);
		atmosphereShader.SetVec3("wavelengthScatter", atmos_config.wavelengthScatter);
		atmosphereShader.SetFloat("scatteringStrength", atmos_config.scatteringStrength);

		atmosphereShader.SetVec3("camPos", camera.transform->world_position);
		atmosphereShader.SetVec3("sunPos", sun->transform->world_position);

		atmosphereShader.SetVec2("screenResolution", window_size);

		atmosphereShader.SetVec3("camUp", camera.transform->up);
		atmosphereShader.SetVec3("camForward", camera.transform->forward);
		atmosphereShader.SetVec3("camRight", camera.transform->right);
		atmosphereShader.SetFloat("camFarPlane", camera.farPlane);
		atmosphereShader.SetFloat("camNearPlane", camera.nearPlane);

		atmosphereShader.SetFloat("FOVdeg", camera.fovDeg);

		atmosphereShader.SetInt("numInScatteringPoints", numInScatteringPoints);
		atmosphereShader.SetInt("numOpticalDepthPoints", numOpticalDepthPoints);


		quad.vao.Bind();
		glDrawElements(GL_TRIANGLES, quad.indices.size(), GL_UNSIGNED_INT, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

	}
}