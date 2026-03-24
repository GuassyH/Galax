#include "Atmosphere.h"

#include "core/Log.h"

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
		atmosphereShader.Compile("assets/shaders/atmosphere.frag", "assets/shaders/atmosphere.vert");

		quad = Mesh(quad_verts, quad_inds);

		glGenBuffers(1, &atmosphereBuffer);
	}

	AtmosphereRenderer::~AtmosphereRenderer() {
		atmosphereShader.Delete();

		quad.Delete();

		glDeleteBuffers(1, &atmosphereBuffer);
	}

	void AtmosphereRenderer::UpdateBuffers() {

		for (auto& config : atmosphere_configs) {
			float scatterR = glm::pow(config.scatteringCoefficient / config.wavelengths.x, 4) * config.scatteringStrength;
			float scatterG = glm::pow(config.scatteringCoefficient / config.wavelengths.y, 4) * config.scatteringStrength;
			float scatterB = glm::pow(config.scatteringCoefficient / config.wavelengths.z, 4) * config.scatteringStrength;

			config.scatteringCoefficients = glm::vec3(scatterR, scatterG, scatterB);
		}

		GLsizeiptr atmBufferSize = sizeof(AtmosphereConfig) * atmosphere_configs.size();
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, atmosphereBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, atmBufferSize, atmosphere_configs.data(), GL_DYNAMIC_COPY);
	}

	void AtmosphereRenderer::Render(Camera& camera, Universe::Planet* sun, GLuint screenTex, GLuint starTex, GLuint depthTex, int w, int h) {

		atmosphereShader.Use();
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, atmosphereBuffer);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, screenTex);
		atmosphereShader.SetInt("screenTexture", 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthTex);
		atmosphereShader.SetInt("depthTexture", 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, starTex);
		atmosphereShader.SetInt("starTexture", 2);


		atmosphereShader.SetInt("numAtmospheres", atmosphere_configs.size());

		atmosphereShader.SetVec3("camPos", camera.transform->world_position);
		atmosphereShader.SetVec3("sunPos", sun->transform->world_position);

		atmosphereShader.SetVec2("screenResolution", glm::vec2(w, h));

		atmosphereShader.SetVec3("camUp", camera.transform->up);
		atmosphereShader.SetVec3("camForward", camera.transform->forward);
		atmosphereShader.SetVec3("camRight", camera.transform->right);
		atmosphereShader.SetFloat("camFarPlane", camera.farPlane);
		atmosphereShader.SetFloat("camNearPlane", camera.nearPlane);

		atmosphereShader.SetFloat("FOVdeg", camera.fovDeg);

		atmosphereShader.SetInt("numInScatteringPoints", numInScatteringPoints);
		atmosphereShader.SetInt("numOpticalDepthPoints", numOpticalDepthPoints);

		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);

		quad.vao.Bind();
		glDrawElements(GL_TRIANGLES, quad.indices.size(), GL_UNSIGNED_INT, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);

	}
}