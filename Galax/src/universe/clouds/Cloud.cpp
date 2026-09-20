#include "Cloud.h"

#include "Camera.h"
#include "rendering/Quad.h"
#include "core/Input.h"
#include "universe/planetary/Planet.h"

namespace Universe {


	CloudRenderer::CloudRenderer() {
		cloudShader.Compile("assets/shaders/universe/cloud.frag", "assets/shaders/universe/cloud.vert");

		quad = Mesh(Quad::vertices, Quad::indices);
	}

	void CloudRenderer::Render(Camera& camera, Universe::Planet* sun, Universe::Planet* planet, CloudConfig& cloud_config, GLuint bakedOpticalDepth, GLuint screenTexture, GLuint depthTexture) {
		glm::vec2 window_size = Galax::InputManager::Get().windowSize;

		cloudShader.Use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bakedOpticalDepth);
		cloudShader.SetInt("bakedOpticalTexture", 0);


		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, screenTexture);
		cloudShader.SetInt("screenTexture", 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		cloudShader.SetInt("depthTexture", 2);


		cloudShader.SetFloat("cloudMin", cloud_config.minHeight + planet->radius);
		cloudShader.SetFloat("cloudMax", cloud_config.maxHeight + planet->radius);

		cloudShader.SetVec3("camPos", camera.transform->world_position);
		cloudShader.SetVec3("sunPos", sun->transform->world_position);

		cloudShader.SetVec2("screenResolution", window_size);

		cloudShader.SetVec3("camUp", camera.transform->up);
		cloudShader.SetVec3("camForward", camera.transform->forward);
		cloudShader.SetVec3("camRight", camera.transform->right);

		cloudShader.SetFloat("FOVdeg", camera.fovDeg);
		cloudShader.SetVec3("centre", planet->transform->world_position);
		cloudShader.SetVec3("sunPos", sun->transform->world_position);
		cloudShader.SetMat4("invProjMat", glm::inverse(camera.GetProj()));

		quad.vao.Bind();
		glDrawElements(GL_TRIANGLES, quad.indices.size(), GL_UNSIGNED_INT, 0);

		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	
}