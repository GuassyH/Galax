#include "Ocean.h"
#include "universe/planetary/Planet.h"

#include "universe/UniverseManager.h"

#include "core/Input.h"
#include "core/Time.h"

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


	OceanRenderer::OceanRenderer() {
		oceanShader.Compile("assets/shaders/universe/ocean.frag", "assets/shaders/universe/ocean.vert");

		quad = Mesh(quad_verts, quad_inds);
	}

	OceanRenderer::~OceanRenderer() {
		oceanShader.Delete();

		quad.Delete();
	}

	void OceanRenderer::Render(Camera& camera, Universe::Planet* sun, Transform* planet, OceanConfig& ocean_config, GLuint colorTex, GLuint depthTex) {
		oceanShader.Use();

		glm::vec2 window_size = Galax::InputManager::Get().windowSize;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthTex);
		oceanShader.SetInt("depthTexture", 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, colorTex);
		oceanShader.SetInt("screenTexture", 1);

		if (ocean_config.normalTexture) {
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, ocean_config.normalTexture->ID);
			oceanShader.SetInt("normalTexture", 2);
		}

		oceanShader.SetBool("hasNormalTex", ocean_config.normalTexture != nullptr);
		oceanShader.SetFloat("normalRepeat", ocean_config.normalRepeat);
		oceanShader.SetFloat("normalStrength", ocean_config.normalStrength);
		oceanShader.SetInt("normalFactor", ocean_config.normalFactor);

		oceanShader.SetFloat("oceanRadius", ocean_config.radius);
		oceanShader.SetFloat("densityFalloff", ocean_config.densityFalloff);
		oceanShader.SetVec4("oceanColor", ocean_config.oceanColor);
		oceanShader.SetVec3("centre", planet->world_position);

		oceanShader.SetMat4("modelMat", planet->GetMatrix());

		oceanShader.SetVec3("camPos", camera.transform->world_position);
		oceanShader.SetVec3("sunPos", sun->transform->world_position);

		oceanShader.SetVec2("screenResolution", glm::vec2(window_size.x, window_size.y));

		oceanShader.SetVec3("camUp", camera.transform->up);
		oceanShader.SetVec3("camForward", camera.transform->forward);
		oceanShader.SetVec3("camRight", camera.transform->right);
		oceanShader.SetFloat("camFarPlane", camera.farPlane);
		oceanShader.SetFloat("camNearPlane", camera.nearPlane);

		oceanShader.SetFloat("FOVdeg", camera.fovDeg);

		oceanShader.SetMat4("viewMat", camera.GetView());
		oceanShader.SetMat4("projMat", camera.GetProj());

		oceanShader.SetFloat("time", UniverseManager::Get().time);
		oceanShader.SetFloat("triplanarBlend", ocean_config.triplanarBlend);

		quad.vao.Bind();
		glDrawElements(GL_TRIANGLES, quad.indices.size(), GL_UNSIGNED_INT, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

	}

}