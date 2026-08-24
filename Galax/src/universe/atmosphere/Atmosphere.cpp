#include "Atmosphere.h"

#include "universe/planetary/Planet.h"

#include "rendering/Quad.h"

#include "core/Log.h"
#include "core/Input.h"

namespace Universe {



	AtmosphereRenderer::AtmosphereRenderer() {
		atmosphereShader.Compile("assets/shaders/universe/atmosphere.frag", "assets/shaders/universe/atmosphere.vert");
		opticalBakeComp.Compile("assets/shaders/universe/bakeOpticalDepth.comp");

		quad = Mesh(Quad::vertices, Quad::indices);
	}

	AtmosphereRenderer::~AtmosphereRenderer() {
		atmosphereShader.Delete();

		quad.Delete();
	}
	

	void AtmosphereRenderer::Render(Camera& camera, Universe::Planet* sun, Universe::Planet* planet, AtmosphereConfig& atmos_config, GLuint screenTex, GLuint depthTex) {
		glm::vec2 window_size = Galax::InputManager::Get().windowSize;

		atmosphereShader.Use();
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, screenTex);
		atmosphereShader.SetInt("screenTexture", 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthTex);
		atmosphereShader.SetInt("depthTexture", 1);


		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, atmos_config.BakedOpticalTexture);
		atmosphereShader.SetInt("bakedOpticalTexture", 2);

		atmosphereShader.SetFloat("planetRadius", planet->radius);
		atmosphereShader.SetFloat("atmosphereHeight", atmos_config.atmosphereHeight);
		atmosphereShader.SetFloat("intensity", atmos_config.intensity);
		atmosphereShader.SetFloat("densityFalloff", atmos_config.densityFalloff);
		atmosphereShader.SetVec3("centre", planet->transform->world_position);
		atmosphereShader.SetVec3("wavelengthScatter", atmos_config.wavelengthScatter);

		atmosphereShader.SetVec3("camPos", camera.transform->world_position);
		atmosphereShader.SetVec3("sunPos", sun->transform->world_position);

		atmosphereShader.SetVec2("screenResolution", window_size);

		atmosphereShader.SetVec3("camUp", camera.transform->up);
		atmosphereShader.SetVec3("camForward", camera.transform->forward);
		atmosphereShader.SetVec3("camRight", camera.transform->right);
		atmosphereShader.SetFloat("camNearPlane", camera.nearPlane);
		atmosphereShader.SetMat4("invProjMat", glm::inverse(camera.GetProj()));

		atmosphereShader.SetFloat("FOVdeg", camera.fovDeg);

		atmosphereShader.SetInt("numInScatteringPoints", numInScatteringPoints);
		atmosphereShader.SetInt("numOpticalDepthPoints", numOpticalDepthPoints);


		quad.vao.Bind();
		glDrawElements(GL_TRIANGLES, quad.indices.size(), GL_UNSIGNED_INT, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

	}

	void AtmosphereRenderer::BakeOpticalDepth(AtmosphereConfig& atmosphereConfig, float planetRadius) {

		glDeleteTextures(1, &atmosphereConfig.BakedOpticalTexture);

		const int textureWidth = 1024;

		glCreateTextures(GL_TEXTURE_2D, 1, &atmosphereConfig.BakedOpticalTexture);
		glTextureStorage2D(atmosphereConfig.BakedOpticalTexture, 1, GL_RGBA32F, textureWidth, textureWidth);

		glBindImageTexture(
			0,											// image unit
			atmosphereConfig.BakedOpticalTexture,       // texture
			0,											// mip level
			GL_FALSE,									// layered
			0,											// layer
			GL_WRITE_ONLY,								// access
			GL_RGBA32F									// format
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		opticalBakeComp.Use();
	

		opticalBakeComp.SetInt("texSize", textureWidth);
		opticalBakeComp.SetInt("numOutScatteringSteps", numOpticalDepthPoints);
		opticalBakeComp.SetFloat("densityFalloff", atmosphereConfig.densityFalloff);
		opticalBakeComp.SetFloat("atmosphereRadius", (atmosphereConfig.atmosphereHeight + planetRadius) / planetRadius);
		opticalBakeComp.SetFloat("planetRadius", 1.0);


		// NOW dispatch
		glDispatchCompute(textureWidth / 8, textureWidth / 8, 1);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
	}
}